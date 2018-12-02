@ Number word set
@
@ \file   number.asm
@ \author Vincent Hamp
@ \date   27/07/2016

.section .text

/***************************************************************************//**
@ number
@ ( token-addr token-u -- n true | false )
@ Convert token-addr u to a number.
@ Return a flag do indicate whether conversion was successful or not.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE, "number"
    push {lr}

@ tos   token-addr
@ r0    token-addr + token-u
@ r1    token-u
@ r2    base
@ r3    character
@ r4    pointer & return flag
@ r5    sign
    POP_REGS r1
    adds r0, tos, r1                    @ Get token-addr + token-u

@ Check if prefix overwrites base ($, # or %)
.ifdef NUMBER_PREFIX
    ldrb r3, [tos]
    cmp r3, #'#'
    bne 1f
        movs r2, #10                    @ Make base decimal
        ldr r4, =cstring2num_base_10    @ Load pointer to base <=10 conversion
        adds tos, #1                    @ Increment token-addr
        b 2f                            @ Goto check sign

1:  cmp r3, #'$'
    bne 1f
        movs r2, #16                    @ Make base hexadecimal
        ldr r4, =cstring2num_base_36    @ Load pointer to base <=36 conversion
        adds tos, #1                    @ Increment token-addr
        b 2f                            @ Goto check sign

1:  cmp r3, #'%'
    bne 1f
        movs r2, #2                     @ Make base binary
        ldr r4, =cstring2num_base_10    @ Load pointer to base <=10 conversion
        adds tos, #1                    @ Increment token-addr
        b 2f                            @ Goto check sign
.endif

@ Get or set base
1:  ldr r2, =radix
    ldr r2, [r2]

    cmp r2, #10
    bls 1f
        ldr r4, =cstring2num_base_36    @ Load pointer to base <=36 conversion
        b 2f

1:  ldr r4, =cstring2num_base_10        @ Load pointer to base <=10 conversion (by default)

@ Check and set sign
2:  movs r5, #1
    ldrb r3, [tos]
    cmp r3, #'-'
    bne 1f                              @ Goto conversion
        movs r5, #-1                    @ Negative sign
        adds tos, #1                    @ Increment token-addr

@ Conversion
1:  blx r4                              @ Call actual conversion

@ Result
    cmp r4, #0                          @ flag - 0
    bne 1f
        movs tos, #0                    @ Ascii character was not in range, return false
        b 6f                            @ Goto return
1:  mul tos, r1, r5                     @ Multiply result with sign to tos
    PUSH_REGS #-1                       @ return ( -- n true )

@ Return
6:  pop {pc}

/***************************************************************************//**
@ cstring2num_base_10
@ Convert a cstring with bases <=10 into a decimal number.
@ tos   token-addr
@ r0    token-addr + token-u
@ r2    base
@ r4    return flag
@       true:   conversion successful
@       false:  conversion failed
 ******************************************************************************/
.type cstring2num_base_10, %function
cstring2num_base_10:

@ r1    result
@ r3    character
    movs r1, #0                         @ Make room for result
    movs r4, #-1                        @ Set flag true

@ Bytewise conversion
1:  cmp r0, tos                         @ (token-addr + token-u) - token-addr
    bls 6f                              @ Goto return
    ldrb r3, [tos], #1
    subs r3, #'0'                       @ Make number out of ascii

@ Check if numeric
    cmp r3, r2                          @ character - base
    itt hs
    movshs r4, #0                       @ Set flag false
    bhs 6f                              @ Goto return

@ Actual calculation
    muls r1, r2                         @ val = val * base + char
    adds r1, r3
    b 1b                                @ Goto bytewise conversion

@ Return
6:  bx lr

/***************************************************************************//**
@ cstring2num_base_36
@ Convert a cstring with bases <=36 into a decimal number.
@ tos   token-addr
@ r0    token-addr + token-u
@ r2    base
@ r4    return flag
@       true:   conversion successful
@       false:  conversion failed
 ******************************************************************************/
.type cstring2num_base_36, %function
cstring2num_base_36:

@ r1    result
@ r3    character
    movs r1, #0                         @ Make room for result
    movs r4, #-1                        @ Set flag true

@ Bytewise conversion
1:  cmp r0, tos                         @ (token-addr + token-u) - token-addr
    bls 6f                              @ Goto return
    ldrb r3, [tos], #1

@ Check range
    cmp r3, #'A'
    blo 2f                              @ Goto 0 - 9
        adds r12, r2, #55               @ Upper limit
        cmp r3, r12                     @ character - upper limit
        blo 3f                          @ Goto A - ?
            movs r4, #0                 @ Set flag false
            b 6f                        @ Goto return

@ 0 - 9
2:  subs r3, #'0'                       @ Make number out of ascii
    cmp r3, #10                         @ character - 10
    blo 4f                              @ Goto actual calculation
        movs r4, #0                     @ Set flag false
        b 6f                            @ Goto return

@ A - ?
3:  subs r3, #55                        @ Make number out of ascii

@ Actual calculation
4:  muls r1, r2                         @ val = val * base + char
    adds r1, r3
    b 1b                                @ Goto bytewise conversion

@ Return
6:  bx lr

/***************************************************************************//**
@ literal
@ ( x -- )
@ Append the run-time semantics given below to the current definition.
@ Compiles code for a literal x in the memory-space. This could either be:
@ movs (2 bytes)
@ movs.w (4 bytes)
@ movw movt (8 bytes)
@
@ ( -- x )
@ Place x on the stack.
 ******************************************************************************/
WORD FLAG_SKIP, "literal", literal
    push {lr}

    PUSH_TOS
    ldr tos, =0xF8476D04                @ PUSH_TOS opcode
    bl rev_comma                        @ Write opcode

@ movs (t1)
@ tos   x
    cmp tos, #0xFF
    bhi 1f                              @ Goto movs (t2)
        orrs tos, #0x2600               @ Opcode template (can be ored directly into tos)
        bl h_comma                      @ Write opcode
            b 6f                        @ Goto return

@ movs (t2)
@ Pattern 0x00XY00XY
@ tos   x
@ r0    bottom
@ r1    top
1:  ands r0, tos, #0x00FF00FF
    cmp r0, tos
    bne 1f                              @ Goto pattern 0xXY00XY00
        movw r1, #0xFFFF
        ands r0, r1                     @ bottom
        lsrs r1, tos, #16               @ top
        cmp r1, r0                      @ top - bottom
        bne 1f                          @ Goto pattern 0xXY00XY00

        ldr tos, =0xF05F1600            @ Opcode template
        orrs tos, r0                    @ Or imm8 into template
        bl rev_comma                    @ Write opcode
            b 6f                        @ Goto return

@ Pattern 0xXY00XY00
@ tos   x
@ r0    bottom
@ r1    top
1:  ands r0, tos, #0xFF00FF00
    cmp r0, tos
    bne 1f                              @ Goto pattern 0xXYXYXYXY
        movw r1, #0xFFFF
        ands r0, r1                     @ bottom
        lsrs r1, tos, #16               @ top
        cmp r1, r0                      @ top - bottom
        bne 1f                          @ Goto pattern 0xXYXYXYXY

        ldr tos, =0xF05F2600            @ Opcode template
        orrs tos, tos, r0, lsr #8       @ Or imm8 into template
        bl rev_comma                    @ Write opcode
            b 6f                        @ Goto return

@ Pattern 0xXYXYXYXY
@ tos   x
@ r0    rotated copy of tos
1:  movs r0, tos
    rors r0, #16
    cmp r0, tos
    bne 1f                              @ Goto pattern 0x00000XY0
        rors r0, #8
        cmp r0, tos
        bne 1f                          @ Goto pattern 0x00000XY0

        ldr tos, =0xF05F3600            @ Opcode template
        orrs tos, tos, r0, lsr #24      @ Or imm8 into template
        bl rev_comma                    @ Write opcode
            b 6f                        @ Goto return

@ Pattern
@ 0x00000XY0
1:  ands r0, tos, #0x00000FF0
    cmp r0, tos
    bne 1f                              @ Goto pattern 0x0000XY00
        b 2f                            @ Goto 12bit encoding

@ 0x0000XY00
1:  ands r0, tos, #0x0000FF00
    cmp r0, tos
    bne 1f                              @ Goto pattern 0x000XY000
        b 2f                            @ Goto 12bit encoding

@ 0x000XY000
1:  ands r0, tos, #0x000FF000
    cmp r0, tos
    bne 1f                              @ Goto pattern 0x00XY0000
        b 2f                            @ Goto 12bit encoding

@ 0x00XY0000
1:  ands r0, tos, #0x00FF0000
    cmp r0, tos
    bne 1f                              @ Goto pattern 0x0XY00000
        b 2f                            @ Goto 12bit encoding

@ 0x0XY00000
1:  ands r0, tos, #0x0FF00000
    cmp r0, tos
    bne 1f                              @ Goto pattern 0xXY000000
        b 2f                            @ Goto 12bit encoding

@ 0xXY000000
1:  ands r0, tos, #0xFF000000
    cmp r0, tos
    bne 1f                              @ Goto pattern 0xY000000X

@ 12bit encoding
@ r0    imm8
@ r1    counter
2:  movs r0, tos                        @ Make copy of tos
    movs r1, #0

2:  rors r0, #31                        @ Rotate imm8 left
    adds r1, #1                         @ Increment counter
    cmp r1, #31                         @ counter - 31
    bhi 1f                              @ Goto movw movt

        cmp r0, #0xFF                   @ imm8 - 0xFF
        bhi 2b
            ands r2, r0, #0x80          @ MSB set?
            beq 2b

            ldr tos, =0xF05F0600        @ Opcode template
            ands r0, #0x7F              @ imm8<6:0>
            orrs tos, r0                @ Or imm8 into template

            ands r0, r1, 0x1            @ imm8<7>
            orrs tos, tos, r0, lsl #7   @ Or imm8<7> into template

            ands r0, r1, 0xE            @ imm3
            orrs tos, tos, r0, lsl #11  @ Or imm3 into template

            ands r1, #0x10              @ i
            it ne
            orrne tos, #0x4000000

            bl rev_comma                @ Write opcode

            b 6f                        @ Goto return

// TODO ev. noch mvn checkn?

@ movw movt
@ tos   opcode
@ r0    bottom | top
@ r1    intermediate
@ r2    x
1:  movs r2, tos

@ movw
    ldr tos, =0xF2400600                @ Opcode template

    uxth r0, r2, ror #0                 @ bottom
    ands r1, r0, #0xFF                  @ imm8
    orrs tos, r1

    ands r1, r0, #0x700                 @ imm3
    orrs tos, tos, r1, lsl #4

    ands r1, r0, #0x800                 @ i
    orrs tos, tos, r1, lsl #15

    ands r1, r0, #0xF000                @ imm4
    orrs tos, tos, r1, lsl #4

    push {r2}                           @ Save xt
    bl rev_comma                        @ Write opcode
    pop {r2}

@ movt
    PUSH_TOS

    ldr tos, =0xF2C00600                @ Opcode template

    lsrs r0, r2, #16                    @ top
    ands r1, r0, #0xFF                  @ imm8
    orrs tos, r1

    ands r1, r0, #0x700                 @ imm3
    orrs tos, tos, r1, lsl #4

    ands r1, r0, #0x800                 @ i
    orrs tos, tos, r1, lsl #15

    ands r1, r0, #0xF000                @ imm4
    orrs tos, tos, r1, lsl #4

    bl rev_comma                        @ Write opcode

@ Return
6:  pop {pc}
.ltorg
