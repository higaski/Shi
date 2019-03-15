@ Macros
@
@ \file   macros.asm
@ \author Vincent Hamp
@ \date   27/07/2016

@ ------------------------------------------------------------------------------
@ Common
@ ------------------------------------------------------------------------------
.macro SET_IN val
    ldr r0, =in
    movs r1, \val
    str r1, [r0]
.endm

.macro SET_SOURCE c_addr, u
    ldr r0, =src
    movs r1, \c_addr
    movs r2, \u
    strd r1, r2, [r0]
.endm

@ ------------------------------------------------------------------------------
@ Push/pop
@ ------------------------------------------------------------------------------
@ Push top of stack
.macro PUSH_TOS
    str tos, [dsp, #-4]!
.endm

@ Push 8bit constant
.macro PUSH_INT8 val
    PUSH_TOS
    movs tos, \val
.endm

@ Push 16bit constant
.macro PUSH_INT16 val
    PUSH_TOS
    movw tos, \val
.endm

@ Push registers (top ends up in tos)
.macro PUSH_REGS top, from:vararg
    PUSH_TOS                            @ Push top of stack
.ifnb \from
    stmdb dsp!, {\from}                 @ Push registers defined by \from
.endif
    movs tos, \top                      @ Move last register to top of stack
.endm

@ Pop registers (tos ends up in top)
.macro POP_REGS top, to:vararg
    movs \top, tos                      @ Move top of stack to last register
.ifnb \to
    ldmia dsp!, {\to}                   @ Pop into registers defined by \to
.endif
    ldmia dsp!, {tos}                   @ Pop last to top of stack
.endm

@ ------------------------------------------------------------------------------
@ Stacking
@ ------------------------------------------------------------------------------
.macro DEPTH
    ldr r0, =e_shi_stack
    subs r0, dsp
    PUSH_TOS
    lsrs tos, r0, #2
.endm

.macro DROP
    ldmia dsp!, {tos}
.endm

.macro DUP
    PUSH_TOS
.endm

.macro NIP
  adds dsp, #4
.endm

.macro OVER
    PUSH_TOS
    ldr tos, [dsp, #4]
.endm

.macro PICK
    ldr tos, [dsp, tos, lsl #2]
.endm

.macro Q_DUP
    cmp tos, #0
    it ne
    strne tos, [dsp, #-4]!
.endm

.macro R_FROM
    PUSH_TOS
    pop {tos}
.endm

.macro ROT
    ldmia dsp!, {r0, r1}
    subs dsp, #8
    str r0, [dsp, #4]
    str tos, [dsp]
    movs tos, r1
.endm

.macro SWAP
    ldr r0, [dsp]
    str tos, [dsp]
    movs tos, r0
.endm

.macro TO_R
    push {tos}
    DROP
.endm

.macro TUCK
    ldmia dsp!, {r0}
    subs dsp, #8
    str tos, [dsp, #4]
    str r0, [dsp]
.endm

.macro TWO_DROP
    adds dsp, #4
    DROP
.endm

.macro TWO_DUP
    ldr r0, [dsp]
    PUSH_TOS
    subs dsp, #4
    str r0, [dsp]
.endm

.macro TWO_NIP
    ldmia dsp!, {r0, r1, r2}
    subs dsp, #4
    str r0, [dsp]
.endm

.macro TWO_OVER
    ldr r0, [dsp, #8]
    PUSH_TOS
    subs dsp, #4
    str r0, [dsp]
    ldr tos, [dsp, #12]
.endm

.macro TWO_R_FROM
    pop {r0, r1}
    PUSH_REGS top=r0, from=r1
.endm

.macro TWO_SWAP
    ldmia dsp!, {r0, r1, r2}
    subs dsp, #4
    str r0, [dsp]
    PUSH_TOS
    subs dsp, #4
    str r2, [dsp]
    movs tos, r1
.endm

.macro TWO_TO_R
    movs r0, tos
    ldmia dsp!, {r1, tos}
    push {r0, r1}
.endm

@ ------------------------------------------------------------------------------
@ Align
@ ------------------------------------------------------------------------------
@ Align register to 2-byte
.macro P2ALIGN1 align, scratch:vararg
.ifnb \scratch
    ands \scratch, \align, #1
.else
    ands r0, \align, #1
.endif
    it ne
    addne \align, #1
.endm

@ Align register to 4-byte
.macro P2ALIGN2 align, scratch:vararg
    P2ALIGN1 \align, \scratch
.ifnb \scratch
    ands \scratch, \align, #2
.else
    ands r0, \align, #2
.endif
    it ne
    addne \align, #2
.endm

@ Align register to 8-byte
.macro P2ALIGN3 align, scratch:vararg
    P2ALIGN2 \align, \scratch
.ifnb \scratch
    ands \scratch, \align, #7
.else
    ands r0, \align, #7
.endif
    it ne
    addne \align, #4
.endm

@ ------------------------------------------------------------------------------
@ Context switch
@ ------------------------------------------------------------------------------
@ Restore forth context
.macro ENTRY
    ldr r12, =s_shi_context
    ldmia r12, {tos, dsp, lfp}
.endm

@ Save forth context
.macro EXIT
    ldr r12, =s_shi_context
    stmia r12, {tos, dsp, lfp}
.endm

@ ------------------------------------------------------------------------------
@ Dictionary
@ This macros are used to create the core-dictionary words in flash. The beauty
@ of those definitions is, that each entry automatically links to the next one.
@ In case the label definition is left blank, the name of the word is used as
@ label.
@ Both, link and name are aligned to even addresses. That's specially important
@ after the words name since that's where the OP-codes begin.
@
@ Structure
@ Link (4 byte)     Points to the next entry
@ Flags (1 byte)
@ Length (1 byte)   Length of counted string
@ Name (cstring)
@ ------------------------------------------------------------------------------
.equ LINK_INVALID, ERASED_WORD          @ Singed comparison to LINK_INVALD decides if link in search is ok or not

.macro WORD flags, name, label
    .p2align 1                          @ Align before link
link\@\():                              @ Label the link
9:  .word 9f                            @ Link (4 byte)
    .byte \flags                        @ Flags (1 byte)
    .byte 8f - 7f                       @ Length (1 byte)
7:  .ascii "\name"                      @ Name (cstring)
8:  .p2align 1                          @ Align before code
.thumb_func
.ifnb \label                            @ Label for code (use name if label wasn't defined)
\label\():
.else
\name\():
.endif
.endm

@ Last word of core dictionary
.macro WORD_TAIL flags
    .p2align 1
link\@\():
9:  .word ERASED_WORD                   @ Last link in core dictionary points to user flash
    .byte \flags
.endm

@ ------------------------------------------------------------------------------
@ Flags
@ bit   description
@ 7     interpretable
@ 6     compileable
@ 5     immediate
@ 4     inline
@ 3     reserve ram bit1
@ 2     reserve ram bit0
@ 1     foldable bit1
@ 0     foldable bit0
@ ------------------------------------------------------------------------------
@ Skip
@ Word is skipped by find
.equ FLAG_SKIP, 0b11111111

@ Interpret
@ Word is interpreted in interpret state
@ Word is restricted in compile state
.equ BIT_INTERPRET, 0b01111111
.equ FLAG_INTERPRET, BIT_INTERPRET

@ Compile
@ Word is restricted in interpret state
@ Word is compiled in compile state
.equ BIT_COMPILE, 0b10111111
.equ FLAG_COMPILE, BIT_COMPILE

@ Interpret | Compile
@ Word is interpreted in interpret state
@ Word is compiled in compile state
.equ FLAG_INTERPRET_COMPILE, BIT_INTERPRET & BIT_COMPILE

@ Immediate | Compile
@ Word is restricted in interpret state
@ Word is interpreted in compile state
.equ BIT_IMMEDIATE, 0b11011111
.equ FLAG_IMMEDIATE, BIT_IMMEDIATE
.equ FLAG_COMPILE_IMMEDIATE, BIT_COMPILE & BIT_IMMEDIATE

@ Interpret | Compile | Immediate
@ Word is interpreted in interpret state
@ Word is interpreted in compile state
.equ FLAG_INTERPRET_COMPILE_IMMEDIATE, BIT_INTERPRET & BIT_COMPILE & BIT_IMMEDIATE

@ Inline
@ Word is inlineable, instead of a call to the definition its code is copied
.equ FLAG_INLINE, 0b11101111

@ Foldable
.equ BIT_FOLDS, 0b00000011
.equ FOLDS_1, ~1
.equ FOLDS_2, ~2
.equ FOLDS_3, ~3

@ Reserve ram
.equ BIT_RESERVE_RAM, 0b00001100
.equ RESERVE_1CELL, ~1 << 2
.equ RESERVE_2CELL, ~2 << 2
.equ RESERVE_3CELL, ~3 << 2

@ ------------------------------------------------------------------------------
@ Print messages
@ ------------------------------------------------------------------------------
.macro PRINT msg
.if ENABLE_PRINT == 1
    mov r0, pc
    b 8f
    .asciz "\msg\n"
    .p2align 1
8:  bl shi_printf
.endif
.endm
