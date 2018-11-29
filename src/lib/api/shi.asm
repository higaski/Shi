/***************************************************************************//**
 *  @brief
 *
 *  @file       dict.asm
 *  @author     Vincent Hamp
 *  @date       27/07/2016
 ******************************************************************************/

.syntax unified
.thumb
.arch armv7e-m

.section .text

@ C/C++ interface
.global shi_init                        @ Initialize shi
.global shi_c_variable                  @ shi c variable
.global shi_evaluate                    @ shi evaluate

/***************************************************************************//**
@ Register mapping
 ******************************************************************************/
@ Map top of stack and stackpointer to last low registers and literal-folding
@ pointer to first high register (hardwired, can not be changed)
tos .req r6                             @ Top of stack
dsp .req r7                             @ Data-stack pointer
lfp .req r8                             @ Literal-folding pointer

/***************************************************************************//**
@ Common
 ******************************************************************************/
#include "config.h"

@ C -> assembler
.equ DSTACK_SIZE, SHI_DSTACK_SIZE
.equ ERASED_WORD, SHI_ERASED_WORD
.equ NUMBER_PREFIX, SHI_NUMBER_PREFIX
.equ TRACE_ENABLED, SHI_TRACE_ENABLED

.include "stm32l431.asm"
.include "macros.asm"

/***************************************************************************//**
@ Dictionary
 ******************************************************************************/
WORD        FLAG_SKIP, "_s_shi"
_s_shi_dict:                            @ Start of dictionaty

.include "core.asm"
.include "number.asm"
.include "extension.asm"

_e_shi_dict:                            @ End of dictionary
WORD_TAIL   FLAG_SKIP, "_e_shi"

/***************************************************************************//**
@ Initialize shi
@ r0    ram start address
@ r1    ram end address
 ******************************************************************************/
shi_init:
    push {r4-r9, lr}

@ Store ram addresses ----------------------------------------------------------
    ldr r2, =ram_begin                  @ Store ram start address
    str r0, [r2]
    ldr r2, =ram_end                    @ Store ram end address
    str r1, [r2]

@ Fill ram ---------------------------------------------------------------------
    bl fill_ram

@ Set memory-space pointers ----------------------------------------------------

@ Flash
    bl set_memory_space_pointer_flash

@ Reserve ram ------------------------------------------------------------------
    bl reserve_ram

@ Set tos dsp and lfp ----------------------------------------------------------
    movs lfp, #0                        @ Put zero into lfp...
    movs tos, #'*'                      @ Put stars onto tos ;)
    ldr dsp, =_e_shi_dstack             @ Reset data-stack pointer

@ Return -----------------------------------------------------------------------
    EXIT                                @ Store context
    pop {r4-r9, pc}

/***************************************************************************//**
@ Fill ram
 ******************************************************************************/
fill_ram:
@ r0    ram start address
@ r1    ram end address
@ r2    erased word
    movs r2, #ERASED_WORD
1:  cmp r0, r1
    beq 2f                              @ Goto set memory-space pointers
        str r2, [r0], #4
        b 1b

@ Return -----------------------------------------------------------------------
2:  bx lr

/***************************************************************************//**
@ Set memory-space pointer for flash
@ Search flash from end to start to find first free flash address
 ******************************************************************************/
set_memory_space_pointer_flash:
@ r0    flash start address
@ r1    flash end address
@ r2    flash content
    ldr r0, =FLASH_START
    ldr r1, =FLASH_END
1:  cmp r0, r1
    bhi 2f
        ldr r2, [r1, #-4]!
        cmp r2, #ERASED_WORD            @ Flash content - erased word
        beq 1b
            adds r1, #4                 @ Written content found, add #4 to account for it then align
            cmp r0, r1
            bne 1f
                TRACE_WRITE "'shi' flash is full, could not set memory-space pointer >>>shi_set_memory_space_pointer_flash<<<"
                b 3f
1:          .if !(FLASH_WRITE_SIZE - 1)
            nop
            .elseif !(FLASH_WRITE_SIZE - 2)
            P2ALIGN1 align=r1, scratch=r2
            .elseif !(FLASH_WRITE_SIZE - 4)
            P2ALIGN2 align=r1, scratch=r2
            .elseif !(FLASH_WRITE_SIZE - 8)
            P2ALIGN3 align=r1, scratch=r2
            .elseif !(FLASH_WRITE_SIZE - 16)
            nop // TODO P2ALIGN4
            .endif
            ldr r0, =flash_begin        @ Store aligned first free flash address
            str r1, [r0]
            b 3f

@ Whole searched flash area was empty, align FLASH_START and store it
2:  .if !(FLASH_WRITE_SIZE - 1)
    nop
    .elseif !(FLASH_WRITE_SIZE - 2)
    P2ALIGN1 align=r0, scratch=r2
    .elseif !(FLASH_WRITE_SIZE - 4)
    P2ALIGN2 align=r0, scratch=r2
    .elseif !(FLASH_WRITE_SIZE - 8)
    P2ALIGN3 align=r0, scratch=r2
    .elseif !(FLASH_WRITE_SIZE - 16)
    nop // TODO P2ALIGN4
    .endif
    ldr r1, =flash_begin                @ Store aligned first free flash address
    str r0, [r1]

@ Return -----------------------------------------------------------------------
3:  bx lr

/***************************************************************************//**
@ Reserve ram
@ Search through flash dictionary to look for definitions which need to reserve
@ ram.
 ******************************************************************************/
reserve_ram:
@ r0    link
@ r1    copy of link
@ r2    flags
@ r3    ram_end address
@ r4    ram_end
    ldr r0, =_s_shi_dict
    ldr r3, =ram_end
    ldr r4, [r3]
    movs r1, r0
1:  ldrb r2, [r0, #4]                   @ Flags
    mvns r2, r2                         @ Invert flags
    ands r2, r2, #BIT_RESERVE_RAM       @ Extract reserve ram bits from flags
    subs r4, r2                         @ If bits are set subtract amount of bytes from ram_end
    ldr r0, [r0]                        @ Link
    cmp r0, #LINK_INVALID               @ End of dictionary?
    itt ne
    movne r1, r0
    bne 1b                              @ Goto search
        str r4, [r3]                    @ Store ram_end

@ Return -----------------------------------------------------------------------
    bx lr

/***************************************************************************//**
@ Forth C variable
@ r0    c-addr  (cstring address)
@ r1    u       (cstring length)
 ******************************************************************************/
shi_c_variable:
    push {r4-r9, lr}

@ Check if string length is reasonable (>0) ------------------------------------
    cmp r1, #0
    bne 1f                              @ Goto enter forth
        TRACE_WRITE "'shi' attempt to evaluate zero-length string >>>shi_c_variable<<<"
        b 2f                            @ Goto return

@ Enter forth ------------------------------------------------------------------
1:  ENTRY                               @ Restore context

@ Store source -----------------------------------------------------------------
    ldr r2, =src
    stmia r2, {r0, r1}

@ Set >IN 0 --------------------------------------------------------------------
    SET_IN #0                           @ Set >in zero

@ C variable -------------------------------------------------------------------
    bl c_variable

@ Leave forth ------------------------------------------------------------------
    EXIT                                @ Store context

@ Return -----------------------------------------------------------------------
2:  pop {r4-r9, pc}

/***************************************************************************//**
@ Forth evaluate
@ r0    c-addr  (cstring address)
@ r1    u       (cstring length)
 ******************************************************************************/
shi_evaluate:
    push {r4-r9, lr}

@ Check if string length is reasonable (>0) ------------------------------------
    cmp r1, #0
    bne 1f                              @ Goto enter forth
        TRACE_WRITE "'shi' attempt to evaluate zero-length string >>>shi_evaluate<<<"
        b 2f                            @ Goto return

@ Enter forth ------------------------------------------------------------------
1:  ENTRY                               @ Restore context

@ Call evaluate ----------------------------------------------------------------
    PUSH_REGS top=r1, from=r0           @ ( -- c-addr u )
    bl evaluate

@ Leave forth ------------------------------------------------------------------
    EXIT                                @ Store context

@ Return -----------------------------------------------------------------------
2:  pop {r4-r9, pc}

/***************************************************************************//**
@ Clear stack
 ******************************************************************************/
clear:
@ tos   0
@ dsp   stack end address
@ r0    stack start address
    movs tos, #0
    ldr dsp, =_e_shi_dstack
    ldr r0, =_s_shi_dstack

1:  cmp r0, dsp
    beq 2f                              @ Goto return
        str tos, [r0], #4
        b 1b

@ Return -----------------------------------------------------------------------
2:  movs tos, #'*'
    bx lr

/***************************************************************************//**
@ trace_write_itm
@ lr    cstring
@       The rather strange way of passing a cstring by lr was necessary because
@       the first implementation, which was relying on numbered macro labels
@       (\@), kept screwing with the way local labels (1, 2, etc.) worked.
 ******************************************************************************/
trace_write_itm:
@ r0    c-addr + u
@ r1    ITM_PORT register address
@ r2    ITM_PORT
@ r3    character

    ldrb r0, [lr, #-1]                  @ u
    adds r0, r0, lr                     @ c-addr + u
    movs r1, #ITM_PORT                  @ ITM port

1:  cmp lr, r0                          @ c-addr - (c-addr + u)
    beq 3f                              @ Goto return
2:      ldr r2, [r1]
        cmp r2, #0                      @ ITM port - 0
        beq 2b
            ldrb r3, [lr], #1           @ Load char from string
            strb r3, [r1]               @ Write char to ITM
            b 1b

3:  P2ALIGN1 lr
    adds lr, #1                         @ Ensure thumb mode (LSB of lr must be set)
    bx lr
.ltorg
