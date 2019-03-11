@ Number word set
@
@ \file   number.asm
@ \author Vincent Hamp
@ \date   27/07/2016

.section .text

@ ------------------------------------------------------------------------------
@ number
@ ( token-addr token-u -- n true | false )
@ Convert token-addr u to a number.
@ Return a flag do indicate whether conversion was successful or not.
@ ------------------------------------------------------------------------------
.thumb_func
number:
    push {r4, lr}

@ r0    token-addr + token-u
@ r1    token-u
@ r2    base
@ r3    character
@ r4    pointer & return flag
@ tos   token-addr
@ r12   sign
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
2:  movs r12, #1
    ldrb r3, [tos]
    cmp r3, #'-'
    bne 1f                              @ Goto conversion
        movs r12, #-1                   @ Negative sign
        adds tos, #1                    @ Increment token-addr

@ Conversion
1:  blx r4                              @ Call actual conversion

@ Result
    cmp r4, #0                          @ flag - 0
    bne 1f
        movs tos, #0                    @ Ascii character was not in range, return false
        b 6f                            @ Goto return
1:  mul tos, r1, r12                    @ Multiply result with sign to tos
    PUSH_REGS #-1                       @ return ( -- n true )

@ Return
6:  pop {r4, pc}

@ ------------------------------------------------------------------------------
@ cstring2num_base_10
@ Convert a cstring with bases <=10 into a decimal number.
@ r0    token-addr + token-u
@ r2    base
@ r4    return flag
@       true:   conversion successful
@       false:  conversion failed
@ tos   token-addr
@ ------------------------------------------------------------------------------
.thumb_func
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

@ ------------------------------------------------------------------------------
@ cstring2num_base_36
@ Convert a cstring with bases <=36 into a decimal number.
@ r0    token-addr + token-u
@ r2    base
@ r4    return flag
@       true:   conversion successful
@       false:  conversion failed
@ tos   token-addr
@ ------------------------------------------------------------------------------
.thumb_func
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
