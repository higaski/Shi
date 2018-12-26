.section .text

/***************************************************************************//**
@ b,
@ ( orig dest -- )
@ Compile an unconditional jump from orig to dest. For future-proofness the
@ 32bit encoding t4 is used as instruction.
 ******************************************************************************/
.thumb_func
b_comma:
    push {lr}

@ tos   orig
@ r0    dest
@ r1    pc-relative address (dest - (orig + 4))
    POP_REGS r0                         @ ( dest -- )
    subs r1, r0, tos                    @ dest - orig
    subs r1, #4                         @ pc is 4 bytes ahead in thumb/thumb2!

@ Range check for b
@ r1    pc-relative address (dest - (orig + 4))
    cmp r1, #-16777216                  @ pc-relative address - -16777216
    bge 1f
        DROP                            @ ( orig -- )
        PRINT "'shi' branch offset too far negative >>>b,<<<"
        b 6f                            @ Goto return

1:  ldr r2, =16777214
    cmp r1, r2                          @ pc-relative address - 16777214
    ble 1f                              @ Goto temporarily set data_begin to orig if necessary
        DROP                            @ ( orig -- )
        PRINT "'shi' branch offset too far positive >>>b,<<<"
        b 6f                            @ Goto return

@ Temporarily set data_begin to orig if necessary
@ tos   orig
@ r0    dest
@ r2    data_begin address
@ r3    data_begin
@ r4    flag to indicate whether data_begin is overwritten or not
1:  movs r4, #0                         @ Reset flag
    ldr r2, =data_begin
    ldr r3, [r2]
    cmp tos, r3
    bhs 1f
        movs r4, #1                     @ Set flag
        str tos, [r2]                   @ Temporarily store orig as data_begin
        movs tos, r3
        PUSH_TOS                        @ ( -- data_begin )

@ b
@ tos   opcode
@ r1    pc-relative address (dest - (orig + 4))
@ r2    J1 | J2 | imm11 | imm10
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
@ r4    flag to indicate whether data_begin is overwritten or not
    cmp r4, #0
    bne 1f
        bl rev_comma                    @ Write opcode
        b 6f

@ Write opcode and reset data_begin
@ r0    data_begin address
1:  bl rev_comma                        @ Write opcode
    ldr r0, =data_begin
    str tos, [r0]
    DROP                                @ ( data_begin -- )

@ Return
6:  pop {pc}

/***************************************************************************//**
@ beq,
@ ( orig dest -- )
@ Compile a conditional equal jump from orig to dest. For future-proofness the
@ 32bit encoding t3 is used as instruction.
 ******************************************************************************/
.thumb_func
beq_comma:
    push {lr}

@ tos   orig
@ r0    dest
@ r1    pc-relative address (dest - (orig + 4))
    POP_REGS r0                         @ ( dest -- )
    subs r1, r0, tos                    @ dest - orig
    subs r1, #4                         @ pc is 4 bytes ahead in thumb/thumb2!

@ Range check for beq
@ r1    pc-relative address (dest - (orig + 4))
    cmp r1, #-1048576                   @ pc-relative address - -1048576
    bge 1f
        DROP                            @ ( orig -- )
        PRINT "'shi' conditional branch offset too far negative >>>beq,<<<"
        b 6f                            @ Goto return

1:  ldr r2, =1048574
    cmp r1, r2                          @ pc-relative address - 1048574
    ble 1f                              @ Goto temporarily set data_begin to orig if necessary
        DROP                            @ ( orig -- )
        PRINT "'shi' conditional branch offset too far positive >>>beq,<<<"
        b 6f                            @ Goto return

@ Temporarily set data_begin to orig if necessary
@ tos   orig
@ r0    dest
@ r2    data_begin address
@ r3    data_begin
@ r4    flag to indicate whether data_begin is overwritten or not
1:  movs r4, #0                         @ Reset flag
    ldr r2, =data_begin
    ldr r3, [r2]
    cmp tos, r3
    bhs 1f
        movs r4, #1                     @ Set flag
        str tos, [r2]                   @ Temporarily store orig as data_begin
        movs tos, r3
        PUSH_TOS                        @ ( -- data_begin)

@ beq
@ tos   opcode
@ r1    pc-relative address (dest - (orig + 4))
@ r2    J2 | J1 | imm11| imm6
1:  ldr tos, =0xF0008000                @ Opcode template

    cmp r1, #0
    it lt
    orrlt tos, #0x4000000               @ Set sign

    ands r2, r1, #0x80000               @ J2
    it ne
    orrne tos, #0x800

    ands r2, r1, #0x40000               @ J1
    it ne
    orrne tos, #0x2000

    lsrs r1, #1
    movw r2, #0x7FF                     @ Mask for imm11
    ands r2, r1                         @ imm11
    orrs tos, r2                        @ Or imm11 into template

    lsrs r1, #11
    ands r2, r1, #0x3F                  @ Mask for imm6
    orrs tos, tos, r2, lsl #16          @ Or imm6 into template

@ Write opcode, do not reset data_begin
@ r4    flag to indicate whether data_begin is overwritten or not
    cmp r4, #0
    bne 1f
        bl rev_comma                    @ Write opcode
        b 6f

@ Write opcode and reset data_begin
@ r0    data_begin address
1:  bl rev_comma                        @ Write opcode
    ldr r0, =data_begin
    str tos, [r0]
    DROP                                @ ( data_begin -- )

@ Return
6:  pop {pc}

/***************************************************************************//**
@ blt,
@ ( orig dest -- )
@ Compile a conditional less-than jump from orig to dest. For future-proofness
@ the 32bit encoding t3 is used as instruction.
 ******************************************************************************/
.thumb_func
blt_comma:
    push {lr}

@ tos   orig
@ r0    dest
@ r1    pc-relative address (dest - (orig + 4))
    POP_REGS r0                         @ ( dest -- )
    subs r1, r0, tos                    @ dest - orig
    subs r1, #4                         @ pc is 4 bytes ahead in thumb/thumb2!

@ Range check for blt
@ r1    pc-relative address (dest - (orig + 4))
    cmp r1, #-1048576                   @ pc-relative address - -1048576
    bge 1f
        DROP                            @ ( orig -- )
        PRINT "'shi' conditional branch offset too far negative >>>blt,<<<"
        b 6f                            @ Goto return

1:  ldr r2, =1048574
    cmp r1, r2                          @ pc-relative address - 1048574
    ble 1f                              @ Goto temporarily set data_begin to orig if necessary
        DROP                            @ ( orig -- )
        PRINT "'shi' conditional branch offset too far positive >>>blt,<<<"
        b 6f                            @ Goto return

@ Temporarily set data_begin to orig if necessary
@ tos   orig
@ r0    dest
@ r2    data_begin address
@ r3    data_begin
@ r4    flag to indicate whether data_begin is overwritten or not
1:  movs r4, #0                         @ Reset flag
    ldr r2, =data_begin
    ldr r3, [r2]
    cmp tos, r3
    bhs 1f
        movs r4, #1                     @ Set flag
        str tos, [r2]                   @ Temporarily store orig as data_begin
        movs tos, r3
        PUSH_TOS                        @ ( -- data_begin)

@ blt
@ tos   opcode
@ r1    pc-relative address (dest - (orig + 4))
@ r2    J2 | J1 | imm11| imm6
1:  ldr tos, =0xF2C08000                @ Opcode template

    cmp r1, #0
    it lt
    orrlt tos, #0x4000000               @ Set sign

    ands r2, r1, #0x80000               @ J2
    it ne
    orrne tos, #0x800

    ands r2, r1, #0x40000               @ J1
    it ne
    orrne tos, #0x2000

    lsrs r1, #1
    movw r2, #0x7FF                     @ Mask for imm11
    ands r2, r1                         @ imm11
    orrs tos, r2                        @ Or imm11 into template

    lsrs r1, #11
    ands r2, r1, #0x3F                  @ Mask for imm6
    orrs tos, tos, r2, lsl #16          @ Or imm6 into template

@ Write opcode, do not reset data_begin
@ r4    flag to indicate whether data_begin is overwritten or not
    cmp r4, #0
    bne 1f
        bl rev_comma                    @ Write opcode
        b 6f

@ Write opcode and reset data_begin
@ r0    data_begin address
1:  bl rev_comma                        @ Write opcode
    ldr r0, =data_begin
    str tos, [r0]
    DROP                                @ ( data_begin -- )

@ Return
6:  pop {pc}

/***************************************************************************//**
@ bne,
@ ( orig dest -- )
@ Compile a conditional not-equal jump from orig to dest. For future-proofness
@ the 32bit encoding t3 is used as instruction.
 ******************************************************************************/
.thumb_func
bne_comma:
    push {lr}

@ tos   orig
@ r0    dest
@ r1    pc-relative address (dest - (orig + 4))
    POP_REGS r0                         @ ( dest -- )
    subs r1, r0, tos                    @ dest - orig
    subs r1, #4                         @ pc is 4 bytes ahead in thumb/thumb2!

@ Range check for bne
@ r1    pc-relative address (dest - (orig + 4))
    cmp r1, #-1048576                   @ pc-relative address - -1048576
    bge 1f
        DROP                            @ ( orig -- )
        PRINT "'shi' conditional branch offset too far negative >>>bne,<<<"
        b 6f                            @ Goto return

1:  ldr r2, =1048574
    cmp r1, r2                          @ pc-relative address - 1048574
    ble 1f                              @ Goto temporarily set data_begin to orig if necessary
        DROP                            @ ( orig -- )
        PRINT "'shi' conditional branch offset too far positive >>>bne,<<<"
        b 6f                            @ Goto return

@ Temporarily set data_begin to orig if necessary
@ tos   orig
@ r0    dest
@ r2    data_begin address
@ r3    data_begin
@ r4    flag to indicate whether data_begin is overwritten or not
1:  movs r4, #0                         @ Reset flag
    ldr r2, =data_begin
    ldr r3, [r2]
    cmp tos, r3
    bhs 1f
        movs r4, #1                     @ Set flag
        str tos, [r2]                   @ Temporarily store orig as data_begin
        movs tos, r3
        PUSH_TOS                        @ ( -- data_begin)

@ bne
@ tos   opcode
@ r1    pc-relative address (dest - (orig + 4))
@ r2    J2 | J1 | imm11| imm6
1:  ldr tos, =0xF0408000                @ Opcode template

    cmp r1, #0
    it lt
    orrlt tos, #0x4000000               @ Set sign

    ands r2, r1, #0x80000               @ J2
    it ne
    orrne tos, #0x800

    ands r2, r1, #0x40000               @ J1
    it ne
    orrne tos, #0x2000

    lsrs r1, #1
    movw r2, #0x7FF                     @ Mask for imm11
    ands r2, r1                         @ imm11
    orrs tos, r2                        @ Or imm11 into template

    lsrs r1, #11
    ands r2, r1, #0x3F                  @ Mask for imm6
    orrs tos, tos, r2, lsl #16          @ Or imm6 into template

@ Write opcode, do not reset data_begin
@ r4    flag to indicate whether data_begin is overwritten or not
    cmp r4, #0
    bne 1f
        bl rev_comma                    @ Write opcode
        b 6f

@ Write opcode and reset data_begin
@ r0    data_begin address
1:  bl rev_comma                        @ Write opcode
    ldr r0, =data_begin
    str tos, [r0]
    DROP                                @ ( data_begin -- )

@ Return
6:  pop {pc}

/***************************************************************************//**
@ h,
@ ( h -- )
@ Reserve half a cell of memory-space and store x in that place. If the
@ memory-space pointer is aligned when h, begins execution, it will remain
@ aligned when h, finishes execution. An ambiguous condition exists if the
@ memory-space pointer is not aligned prior to execution of h,.
 ******************************************************************************/
.thumb_func
h_comma:
    ldr r0, =data_begin
    ldr r1, [r0]
    strh tos, [r1], #2                  @ Write h to address in data_begin
    str r1, [r0]                        @ Update address in data_begin
    DROP                                @ ( h -- )
    bx lr

/***************************************************************************//**
@ rev,
@ ( x -- )
@ Reserve one cell of memory-space and store x in reverse order in the cell. If
@ the memory-space pointer is aligned when r, begins execution, it will remain
@ aligned when r, finishes execution. An ambiguous condition exists if the
@ memory-space pointer is not aligned prior to execution of r,.
 ******************************************************************************/
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
