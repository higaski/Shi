/***************************************************************************//**
 *  @brief
 *
 *  @file       dict.asm
 *  @author     Vincent Hamp
 *  @date       27/07/2016
 ******************************************************************************/

.syntax unified
.thumb
.arch armv7-m

@ C/C++ interface
.global shi_init_asm, shi_c_variable_asm, shi_evaluate_asm

.section .text

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
#include "shi.h"

@ C -> assembler
.equ DSTACK_SIZE, SHI_DSTACK_SIZE
.equ ERASED_WORD, SHI_ERASED_WORD
.equ NUMBER_PREFIX, SHI_NUMBER_PREFIX
.equ PRINT_ENABLED, SHI_PRINT_ENABLED

.include "extern.asm"
.include "data.asm"
.include "macros.asm"
.include "interpret.asm"

/***************************************************************************//**
@ Dictionary
 ******************************************************************************/
WORD FLAG_SKIP, "_s_shi"
_s_shi_dict:                            @ Start of dictionaty

.include "core.asm"
.include "number.asm"
.include "extension.asm"

.section .data

_e_shi_dict:                            @ End of dictionary
WORD_TAIL FLAG_SKIP, "_e_shi"

.section .text

/***************************************************************************//**
@ Initialize shi
@ r0    ram_begin
@ r1    ram_end
@ r2    flash_begin
@ r3    flash_end
 ******************************************************************************/
.thumb_func
shi_init_asm:
    push {r4-r9, lr}

@ Store addresses
    ldr r4, =ram_begin                  @ Store addresses
    stmia r4, {r0, r1, r2, r3}

@ Fill ram
    bl fill_ram

@ Set memory-space pointer
    bl set_memory_space_pointer_flash

@ Reserve ram
    bl reserve_ram

@ Set tos dsp and lfp
    movs lfp, #0                        @ Put zero into lfp...
    movs tos, #'*'                      @ Put stars onto tos ;)
    ldr dsp, =_e_shi_dstack             @ Reset data-stack pointer

@ Return
    EXIT                                @ Store context
    pop {r4-r9, pc}

/***************************************************************************//**
@ Fill ram
 ******************************************************************************/
.thumb_func
fill_ram:
@ r0    ram_begin
@ r1    ram_end
@ r2    erased word
    ldr r2, =ram_begin
    ldmia r2, {r0, r1}
    movs r2, #ERASED_WORD
1:  cmp r0, r1
    beq 2f                              @ Goto set memory-space pointers
        str r2, [r0], #4
        b 1b

@ Return
2:  bx lr

/***************************************************************************//**
@ Set memory-space pointer for flash
@ Search flash from end to start to find first free flash address
 ******************************************************************************/
.thumb_func
set_memory_space_pointer_flash:
@ r0    flash start address
@ r1    flash_begin
@ r2    flash_end
@ r3    flash content
    ldr r0, =flash_begin
    ldmia r0, {r1, r2}
    ldr r3, =_e_shi_dict                @ Set last link of core to user dictionary
    str r1, [r3]
1:  cmp r1, r2
    bhi 1f
        ldr r3, [r2, #-4]!
        cmp r3, #ERASED_WORD            @ Flash content - erased word
        beq 1b
            adds r2, #4                 @ Written content found, add #4 to account for it
            str r2, [r0]                @ Store first free flash address

@ Return
1:  bx lr

/***************************************************************************//**
@ Reserve ram
@ Search through flash dictionary to look for definitions which need to reserve
@ ram.
 ******************************************************************************/
.thumb_func
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

@ Return
    bx lr

/***************************************************************************//**
@ Forth C variable
@ r0    c-addr  (cstring address)
@ r1    u       (cstring length)
 ******************************************************************************/
.thumb_func
shi_c_variable_asm:
    push {r4-r9, lr}

@ Check if string length is reasonable (>0)
    cmp r1, #0
    bne 1f                              @ Goto enter forth
        PRINT "'shi' attempt to evaluate zero-length string >>>shi_c_variable<<<"
        b 2f                            @ Goto return

@ Enter forth
1:  ENTRY                               @ Restore context

@ Store source
    ldr r2, =src
    stmia r2, {r0, r1}

@ Set >IN 0
    SET_IN #0                           @ Set >in zero

@ C variable
    bl c_variable

@ Leave forth
    EXIT                                @ Store context

@ Return
2:  pop {r4-r9, pc}

/***************************************************************************//**
@ Forth evaluate
@ r0    c-addr  (cstring address)
@ r1    u       (cstring length)
 ******************************************************************************/
.thumb_func
shi_evaluate_asm:
    push {r4-r9, lr}

@ Check if string length is reasonable (>0)
    cmp r1, #0
    bne 1f                              @ Goto enter forth
        PRINT "'shi' attempt to evaluate zero-length string >>>shi_evaluate<<<"
        b 2f                            @ Goto return

@ Enter forth
1:  ENTRY                               @ Restore context

@ Call evaluate
    PUSH_REGS top=r1, from=r0           @ ( -- c-addr u )
    bl evaluate

@ Leave forth
    EXIT                                @ Store context

@ Return
2:  pop {r4-r9, pc}

/***************************************************************************//**
@ Clear stack
 ******************************************************************************/
.thumb_func
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

@ Return
2:  movs tos, #'*'
    bx lr
.ltorg
