@ Shi
@
@ \file   dict.asm
@ \author Vincent Hamp
@ \date   27/07/2016

.syntax unified
.thumb
.arch armv7-m

@ C/C++ interface
.global shi_init_asm, shi_c_variable_asm, shi_evaluate_asm, shi_clear_asm

.section .text

@ ------------------------------------------------------------------------------
@ Register mapping
@ ------------------------------------------------------------------------------
@ Map top of stack and stackpointer to last low registers and literal-folding
@ pointer to first high register (hardwired, can not be changed)
tos .req r6                             @ Top of stack
dsp .req r7                             @ Data-stack pointer
lfp .req r8                             @ Literal-folding pointer

@ ------------------------------------------------------------------------------
@ Common
@ ------------------------------------------------------------------------------
#include "shi.hpp"

@ C -> assembler
.equ DATASTACK_SIZE, SHI_STACK_SIZE
.equ ERASED_WORD, SHI_ERASED_WORD
.equ NUMBER_PREFIX, SHI_NUMBER_PREFIX
.equ PRINT_ENABLED, SHI_PRINT_ENABLED

.include "extern.asm"
.include "data.asm"
.include "macros.asm"
.include "comma.asm"
.include "interpret.asm"

@ ------------------------------------------------------------------------------
@ Dictionary
@ ------------------------------------------------------------------------------
WORD FLAG_SKIP, "s_shi"
s_shi_dict:                             @ Start of dictionaty

.include "core.asm"
.include "number.asm"
.include "extension.asm"

.section .data

e_shi_dict:                             @ End of dictionary
WORD_TAIL FLAG_SKIP, "e_shi"

.section .text

@ ------------------------------------------------------------------------------
@ Initialize shi
@ r0    init_t address
@ ------------------------------------------------------------------------------
.thumb_func
shi_init_asm:
    push {tos-lfp, lr}

@ Store data addresses
@ r0    init_t address
@ r1    data_begin
@ r2    data_end
@ r3    data_begin address
    ldmia r0!, {r1, r2}
    ldr r3, =data_begin
    stmia r3!, {r1, r2}

@ Store text addresses
@ r0    init_t address
@ r1    text_begin
@ r2    text_end
@ r3    text_begin address
    ldmia r0!, {r1, r2}
    stmia r3, {r1, r2}
    cmp r1, r2
    itt ne
    ldrne r3, =e_shi_dict               @ Set last link of core to user dictionary
    strne r1, [r3]

@ Store text alignment
@ r0    init_t address
@ r1    text_align
@ r2    text_align address
    ldrb r0, [r0]
    movs r1, #1
    lsl r1, r0
    ldr r2, =text_align
    strb r1, [r2]

@ Reserve data and set text_begin to last text link
    bl sweep_text

@ Fill data
    bl fill_data

@ Set tos dsp and lfp
    movs lfp, #0                        @ Put zero into lfp...
    movs tos, #'*'                      @ Put stars onto tos ;)
    ldr dsp, =e_shi_stack               @ Reset data-stack pointer

@ Return
    EXIT                                @ Store context
    pop {tos-lfp, pc}

@ ------------------------------------------------------------------------------
@ Fill data with 0xFF
@ ------------------------------------------------------------------------------
.thumb_func
fill_data:
@ r0    data_begin
@ r1    data_end
@ r2    erased word
    ldr r2, =data_begin
    ldmia r2, {r0, r1}
    movs r2, #ERASED_WORD
1:  cmp r0, r1
    beq 6f
        str r2, [r0], #4 @TODO write last few bytes single... who says data is 4-byte aligned?
        b 1b

@ Return
6:  bx lr

@ ------------------------------------------------------------------------------
@ Search through text dictionary to look for definitions which need to reserve
@ data. Afterwards set text_begin to last found link in text.
@ ------------------------------------------------------------------------------
.thumb_func
sweep_text:
@ r0    link n
@ r1    link n+1
@ r2    flags
@ r3    data_end address
@ r12   data_end
    ldr r1, =s_shi_dict
    ldr r3, =data_end
    ldr r12, [r3]
1:  ldrb r2, [r1, #4]                   @ Flags
    mvns r2, r2                         @ Invert flags
    ands r2, r2, #BIT_RESERVE_RAM       @ Extract reserve ram bits from flags
    subs r12, r2                        @ If bits are set subtract amount of bytes from data_end
    ldr r1, [r1]                        @ Link
    cmp r1, #LINK_INVALID               @ End of dictionary?
    itt ne
    movne r0, r1
    bne 1b                              @ Goto search
        str r12, [r3]                   @ Store data_end
        ldr r1, =text_begin
        str r0, [r1]                    @ text_begin is at last found link

@ Return
    bx lr

@ ------------------------------------------------------------------------------
@ Forth C variable
@ r0    c-addr  (cstring address)
@ r1    u       (cstring length)
@ ------------------------------------------------------------------------------
.thumb_func
shi_c_variable_asm:
    push {tos-lfp, lr}

@ Check if string length is reasonable (>0)
    cmp r1, #0
    bne 1f                              @ Goto enter forth
        PRINT "'shi' attempt to evaluate zero-length string >>>shi_c_variable<<<"
        b 6f                            @ Goto return

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
6:  pop {tos-lfp, pc}

@ ------------------------------------------------------------------------------
@ Forth evaluate
@ r0    c-addr  (cstring address)
@ r1    u       (cstring length)
@ ------------------------------------------------------------------------------
.thumb_func
shi_evaluate_asm:
    push {tos-lfp, lr}

@ Check if string length is reasonable (>0)
    cmp r1, #0
    bne 1f                              @ Goto enter forth
        PRINT "'shi' attempt to evaluate zero-length string >>>shi_evaluate<<<"
        b 6f                            @ Goto return

@ Enter forth
1:  ENTRY                               @ Restore context

@ Call evaluate
    PUSH_REGS top=r1, from=r0           @ ( -- c-addr u )
    bl evaluate

@ Leave forth
    EXIT                                @ Store context

@ Return
6:  pop {tos-lfp, pc}

@ ------------------------------------------------------------------------------
@ Clear stack
@ ------------------------------------------------------------------------------
.thumb_func
shi_clear_asm:
    push {tos-lfp, lr}

@ r0    s_shi_stack
@ tos   0
@ dsp   e_shi_stack
    movs tos, #0
    ldr dsp, =e_shi_stack
    ldr r0, =s_shi_stack
1:  cmp r0, dsp
    beq 6f                              @ Goto return
        str tos, [r0], #4
        b 1b

@ Return
6:  movs lfp, #0                        @ Put zero into lfp...
    movs tos, #'*'                      @ Put stars onto tos ;)
    EXIT
    pop {tos-lfp, pc}
.ltorg
