@ Comma
@
@ \file   comma.asm
@ \author Vincent Hamp
@ \date   27/07/2016

.section .text

@ ------------------------------------------------------------------------------
@ b,
@ ( orig dest -- )
@ Compile an unconditional jump from orig to dest. For future-proofness the
@ 32bit encoding t4 is used as instruction.
@ ------------------------------------------------------------------------------
.thumb_func
b_comma:
    push {lr}

@ Calculate pc-relative address
@ r0    dest
@ r1    pc-relative address (dest - (orig + 4))
@ tos   orig
    POP_REGS r0                         @ ( dest -- )
    subs r1, r0, tos                    @ dest - orig
    subs r1, #4                         @ pc is 4 bytes ahead in thumb/thumb2!

@ Range check
@ r1    pc-relative address (dest - (orig + 4))
    cmp r1, #-16777216                  @ pc-relative address - -16777216
    bge 1f
        DROP                            @ ( orig -- )
        PRINT "b, branch offset too far negative"
        b 6f                            @ Goto return

1:  ldr r2, =16777214
    cmp r1, r2                          @ pc-relative address - 16777214
    ble 1f                              @ Goto temporarily set data_begin to orig if necessary
        DROP                            @ ( orig -- )
        PRINT "b, branch offset too far positive"
        b 6f                            @ Goto return

@ Temporarily set data_begin to orig if necessary
@ r0    dest
@ r2    data_begin address
@ r3    data_begin
@ tos   orig
@ r12   flag to indicate whether data_begin is overwritten or not
1:  movs r12, #0                        @ Reset flag
    ldr r2, =data_begin
    ldr r3, [r2]
    cmp tos, r3
    bhs 1f
        movs r12, #1                    @ Set flag
        str tos, [r2]                   @ Temporarily store orig as data_begin
        push {r3}                       @ ( R: -- data_begin )

@ r1    pc-relative address (dest - (orig + 4))
@ r2    J1 | J2 | imm11 | imm10
@ tos   opcode
1:  ldr tos, =0xF0009000                @ Opcode template

    cmp r1, #0                          @ pc-relative address - 0
    blt 1f                              @ Goto signed

@ Unsigned
    ands r2, r1, #0x800000              @ J1 = !I1
    it eq
    orreq tos, #0x2000

    ands r2, r1, #0x400000              @ J2 = !I2
    it eq
    orreq tos, #0x800
    b 2f

@ Signed
1:  ands r2, r1, #0x800000              @ J1 = I1
    it ne
    orrne tos, #0x2000

    ands r2, r1, #0x400000              @ J2 = I2
    it ne
    orrne tos, #0x800

    orr tos, #0x4000000                 @ Set sign

2:  lsrs r1, #1
    movw r2, #0x7FF                     @ Mask for imm11
    ands r2, r1                         @ imm11
    orrs tos, r2                        @ Or imm11 into template

    lsrs r1, #11
    movw r2, #0x3FF                     @ Mask for imm10
    ands r2, r1                         @ imm10
    orrs tos, tos, r2, lsl #16          @ Or imm10 into template

@ Write opcode, do not reset data_begin
@ r12   flag to indicate whether data_begin is overwritten or not
    cmp r12, #0
    bne 1f
        bl rev_comma                    @ ( opcode -- )
        b 6f

@ Write opcode and reset data_begin
@ r0    data_begin address
@ r3    data_begin
1:  bl rev_comma                        @ ( opcode -- )
    ldr r0, =data_begin
    pop {r3}                            @ ( R: data_begin -- )
    str r3, [r0]

@ Return
6:  pop {pc}

@ ------------------------------------------------------------------------------
@ bc,
@ ( orig dest opcode -- )
@ Compile a conditional equal jump from orig to dest. For future-proofness the
@ 32bit encoding t3 is used as instruction.
@ ------------------------------------------------------------------------------
.thumb_func
bc_comma:
    push {lr}

@ Calculate pc-relative address
@ r0    dest
@ r1    orig
    ldmia dsp!, {r0, r1}                @ ( orig dest opcode -- opcode )
    subs r0, r1                         @ dest - orig
    subs r0, #4                         @ pc is 4 bytes ahead in thumb/thumb2!

@ Range check
@ r0    pc-relative address (dest - (orig + 4))
    cmp r0, #-1048576                   @ pc-relative address - -1048576
    bge 1f
        DROP                            @ ( opcode -- )
        PRINT "bc, conditional branch offset too far negative"
        b 6f                            @ Goto return

1:  ldr r2, =1048574
    cmp r0, r2                          @ pc-relative address - 1048574
    ble 1f                              @ Goto temporarily set data_begin to orig if necessary
        DROP                            @ ( opcode -- )
        PRINT "bc, conditional branch offset too far positive"
        b 6f                            @ Goto return

@ Temporarily set data_begin to orig if necessary
@ r1    orig
@ r2    data_begin address
@ r3    data_begin
@ r12   flag to indicate whether data_begin is overwritten or not
1:  movs r12, #0                        @ Reset flag
    ldr r2, =data_begin
    ldr r3, [r2]
    cmp r1, r3
    bhs 1f
        movs r12, #1                    @ Set flag
        str r1, [r2]                    @ Temporarily store orig as data_begin
        push {r3}                       @ ( R: -- data_begin )

@ r0    pc-relative address (dest - (orig + 4))
@ r2    J2 | J1 | imm11| imm6
@ tos   opcode
1:  cmp r0, #0
    it lt
    orrlt tos, #0x4000000               @ Set sign

    ands r2, r0, #0x80000               @ J2
    it ne
    orrne tos, #0x800

    ands r2, r0, #0x40000               @ J1
    it ne
    orrne tos, #0x2000

    lsrs r0, #1
    movw r2, #0x7FF                     @ Mask for imm11
    ands r2, r0                         @ imm11
    orrs tos, r2                        @ Or imm11 into template

    lsrs r0, #11
    ands r2, r0, #0x3F                  @ Mask for imm6
    orrs tos, tos, r2, lsl #16          @ Or imm6 into template

@ Write opcode, do not reset data_begin
@ r12   flag to indicate whether data_begin is overwritten or not
    cmp r12, #0
    bne 1f
        bl rev_comma                    @ ( opcode -- )
        b 6f

@ Write opcode and reset data_begin
@ r0    data_begin address
@ r3    data_begin
1:  bl rev_comma                        @ ( opcode -- )
    ldr r0, =data_begin
    pop {r3}                            @ ( R: data_begin -- )
    str r3, [r0]

@ Return
6:  pop {pc}

@ ------------------------------------------------------------------------------
@ csp,
@ (                     lvl -- )
@ ( C: leave-sys | case-sys -- )
@ ------------------------------------------------------------------------------
.thumb_func
csp_comma:
    push {r4, lr}

@ Check if csp and dsp clash
@ r4    csp
    ldr r4, =csp
    ldr r4, [r4]
    cmp r4, dsp
    blo 1f
        PRINT "csp, stack overflow"
        b 6f

@ Resolve leave-sys or case-sys
@ r0    leave_lvl | case_lvl
@ r1    leave-sys | case-sys
@ r4    csp
@ r5    flag to indicate that sys can't be resolved
@ tos   lvl
1:  push {r5}
    movs r5, #0
1:  ldr r0, =shi_stack_begin
    cmp r0, r4                          @ csp - shi_stack_begin
    beq 5f
        ldrd r0, r1, [r4, #-8]
        cmp r0, tos
        beq 2f
            cmp r5, #0
            ittt eq
            ldreq r0, =csp              @ Store csp in case sys can't be resolved
            streq r4, [r0]
            moveq r5, #1
            subs r4, #8
            b 1b
2:  PUSH_REGS r1                        @ ( -- sys )
    bl here                             @ ( sys -- sys dest )
    bl b_comma                          @ ( sys dest -- )
    subs r4, #8
    b 1b

@ Store csp in case all sys could be resolved
@ r0    csp address
@ r4    csp
@ r5    flag to indicate that sys can't be resolved
5:  cmp r5, #0                          @ At least one sys couldn't be resolved
    itt eq                              @ Do not write csp
    ldreq r0, =csp
    streq r4, [r0]
    pop {r5}

@ Return
6:  DROP                                @ ( lvl -- )
    pop {r4, pc}

@ ------------------------------------------------------------------------------
@ h,
@ ( h -- )
@ Reserve half a cell of data space and store x in the cell. If the data-space
@ pointer is aligned when h, begins execution, it will remain aligned when h,
@ finishes execution. An ambiguous condition exists if the data-space pointer
@ is not aligned prior to execution of h,.
@ ------------------------------------------------------------------------------
.thumb_func
h_comma:
    ldr r0, =data_begin
    ldr r1, [r0]
    strh tos, [r1], #2                  @ Write h to address in data_begin
    str r1, [r0]                        @ Update address in data_begin
    DROP                                @ ( h -- )
    bx lr

@ ------------------------------------------------------------------------------
@ inline,
@ ( xt -- )
@ Inlines code from xt in the data-space.
@ ------------------------------------------------------------------------------
.thumb_func
inline_comma:
    push {lr}

@ Copy opcodes from xt
@ r0    xt
@ r1    opcode
@ r2    hword
    POP_REGS r0
    movw r1, #0x4770
1:  ldrh r2, [r0], #2
    cmp r2, r1                          @ End if opcode equals bx lr
    beq 6f
        cmp r2, #0xBD00                 @ or pop {pc}
        beq 6f
            PUSH_REGS r2
            push {r0, r1}               @ ( R: -- xt opcode )
            bl h_comma
            pop {r0, r1}                @ ( R: xt opcode -- )
            b 1b

@ Return
6:  pop {pc}

@ ------------------------------------------------------------------------------
@ rev,
@ ( x -- )
@ Reserve one cell of data space and store x in reverse order in the cell. If
@ the data-space pointer is aligned when rev, begins execution, it will remain
@ aligned when rev, finishes execution. An ambiguous condition exists if the
@ data-space pointer is not aligned prior to execution of rev,.
@ ------------------------------------------------------------------------------
.thumb_func
rev_comma:
    ldr r0, =data_begin
    ldr r1, [r0]
    movs r2, tos
    lsrs tos, #16
    strh tos, [r1], #2                  @ Write x to address in data_begin
    strh r2, [r1], #2                   @ Write x to address in data_begin
    str r1, [r0]                        @ Update address in data_begin
    DROP                                @ ( x -- )
    bx lr

@ ------------------------------------------------------------------------------
@ word,
@ ( source: "<spaces>name" -- )
@ Skip leading space delimiters. Parse name delimited by a space. Create a
@ definition for name with the execution semantics defined below. If the
@ data-space pointer is not aligned, reserve enough data space to align it. The
@ new data-space pointer defines name's data field. word_comma does not allocate
@ data space in name's data field.
@ ------------------------------------------------------------------------------
.thumb_func
word_comma:
    push {lr}

@ Parse
    bl source                           @ ( -- c-addr u )
    bl parse_name                       @ ( c-addr u -- token-addr token-u )
    cmp tos, #0                         @ token-u - 0
    bne 1f                              @ Goto find
        TWO_DROP                        @ ( token-addr false -- )
        PRINT "word_comma zero-length string as a name"
        b 6f                            @ Goto return

@ Find
1:  TWO_DUP                             @ ( -- token-addr token-u token-addr token-u )
    bl find                             @ ( -- token-addr 0 | xt flags )
    // TODO maybe write a "create" which doesn't check for redefinition?
    cmp tos, #0                         @ flags - 0
    beq 1f                              @ Goto create
        TWO_DROP                        @ ( xt flags -- )
        TWO_DROP                        @ ( token-addr token-u -- )
        PRINT "word_comma redefined word"
        // TODO What should we do in case a word gets redefined?
        // Maybe src should be dropped, so that when we leave create we also
        // leave evaluate?
        b 6f                            @ Goto return

@ Write link
@ r0    data_begin address
@ r1    data_begin
@ r2    link address
@ r3    link
1:  TWO_DROP                            @ ( token-addr 0 -- )
    ldr r0, =data_begin
    ldr r1, [r0]
    ldr r2, =link
    ldr r3, [r2]
    str r1, [r2]                        @ Update last link
    str r3, [r1], #4                    @ Write link

@ Write flags
@ r0    data_begin address
@ r1    data_begin
@ r2    flags
    movs r2, #FLAG_INTERPRET_COMPILE
    strb r2, [r1], #1

@ Write cstring
@ r0    data_begin address
@ r1    data_begin
@ r2    token-addr
@ r3    token-addr + token-u
@ tos   token-u
@ r12   character
    strb tos, [r1], #1                  @ Write length
    POP_REGS top=r3, to=r2              @ ( token-addr token-u -- )
    adds r3, r3, r2
1:  cmp r3, r2                          @ token-addr + token-u - token-addr
    bls 1f
        ldrb r12, [r2], #1
        strb r12, [r1], #1
        b 1b

@ Align data_begin
@ Name could have been any length and screw with alignment
@ r0    data_begin address
@ r1    data_begin
1:  P2ALIGN1 align=r1, scratch=r2
    str r1, [r0]

@ Return
6:  pop {pc}
