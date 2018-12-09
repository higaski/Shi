.section .text

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
    ble 1f                              @ Goto temporarily set ram_begin to orig if necessary
        DROP                            @ ( orig -- )
        PRINT "'shi' conditional branch offset too far positive >>>blt,<<<"
        b 6f                            @ Goto return

@ Temporarily set ram_begin to orig if necessary
@ tos   orig
@ r0    dest
@ r2    ram_begin address
@ r3    ram_begin
@ r4    flag to indicate whether ram_begin is overwritten or not
1:  movs r4, #0                         @ Reset flag
    ldr r2, =ram_begin
    ldr r3, [r2]
    cmp tos, r3
    bhs 1f
        movs r4, #1                     @ Set flag
        str tos, [r2]                   @ Temporarily store orig as ram_begin
        movs tos, r3
        PUSH_TOS                        @ ( -- ram_begin)

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

@ Write opcode, do not reset ram_begin
@ r4    flag to indicate whether ram_begin is overwritten or not
    cmp r4, #0
    bne 1f
        bl rev_comma                    @ Write opcode
        b 6f

@ Write opcode and reset ram_begin
@ r0    ram_begin address
1:  bl rev_comma                        @ Write opcode
    ldr r0, =ram_begin
    str tos, [r0]
    DROP                                @ ( ram_begin -- )

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
    ble 1f                              @ Goto temporarily set ram_begin to orig if necessary
        DROP                            @ ( orig -- )
        PRINT "'shi' conditional branch offset too far positive >>>bne,<<<"
        b 6f                            @ Goto return

@ Temporarily set ram_begin to orig if necessary
@ tos   orig
@ r0    dest
@ r2    ram_begin address
@ r3    ram_begin
@ r4    flag to indicate whether ram_begin is overwritten or not
1:  movs r4, #0                         @ Reset flag
    ldr r2, =ram_begin
    ldr r3, [r2]
    cmp tos, r3
    bhs 1f
        movs r4, #1                     @ Set flag
        str tos, [r2]                   @ Temporarily store orig as ram_begin
        movs tos, r3
        PUSH_TOS                        @ ( -- ram_begin)

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

@ Write opcode, do not reset ram_begin
@ r4    flag to indicate whether ram_begin is overwritten or not
    cmp r4, #0
    bne 1f
        bl rev_comma                    @ Write opcode
        b 6f

@ Write opcode and reset ram_begin
@ r0    ram_begin address
1:  bl rev_comma                        @ Write opcode
    ldr r0, =ram_begin
    str tos, [r0]
    DROP                                @ ( ram_begin -- )

@ Return
6:  pop {pc}
