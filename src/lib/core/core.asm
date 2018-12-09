@ Core word set
@
@ \file   core.asm
@ \author Vincent Hamp
@ \date   27/07/2016

.section .text

/***************************************************************************//**
@ !
@ ( x a-addr -- )
@ Store x at a-addr.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE, "!", store
    ldmia dsp!, {r0, r1}
    str r0, [tos]
    movs tos, r1
    bx lr

/*
WORD FLAG_SKIP, "#", num
    bx lr
*/

/*
WORD FLAG_SKIP, "#>", num_end
    bx lr
*/

/*
WORD FLAG_SKIP, "#s", num_s
    bx lr
*/

/***************************************************************************//**
@ '
@ ( source: "<spaces>name" --    )
@ (                        -- xt )
@ Skip leading space delimiters. Parse name delimited by a space. Find name and
@ return xt, the execution token for name. An ambiguous condition exists if name
@ is not found. When interpreting, ' xyz execute is equivalent to xyz.
 ******************************************************************************/
WORD FLAG_INTERPRET, "'", tick
    push {lr}

@ Parse
    bl source                           @ ( -- c-addr u )
    bl parse                            @ ( -- token-addr token-u )
    cmp tos, #0                         @ token-u - 0
    bne 1f                              @ Goto find
        TWO_DROP                        @ ( token-addr false -- )
        PRINT "'shi' attempt to use zero-length string as a name >>>'<<<"
        b 6f                            @ Goto return

@ Find
1:  bl find                             @ ( -- token-addr 0 | xt flags )
    cmp tos, #0                         @ flags - 0
    bne 1f                              @ Goto xt
        TWO_DROP                        @ ( token-addr 0 -- )
        PRINT "'shi' undefined word >>>'<<<"
        b 6f                            @ Goto return

@ xt
1:  DROP                                @ ( flags -- )

@ Return
6:  pop {pc}

/*
WORD FLAG_SKIP, "(", comment
    bx lr
*/

/***************************************************************************//**
@ *
@ ( n1 n2 -- n3 )
@ Multiply n1 by n2 giving the product n3
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "*", times
    ldmia dsp!, {r0}
    muls tos, r0
    bx lr

/***************************************************************************//**
@ * /
@ ( n1 n2 n3 -- n4 )
@ Multiply n1 by n2 producing the intermediate double-cell result d. Divide d by
@ n3 giving the single-cell quotient n4. An ambiguous condition exists if n3 is
@ zero or if the quotient n4 lies outside the range of a signed number. If d and
@ n3 differ in sign, the implementation-defined result returned will be the same
@ as that returned by either the phrase >r m* r> fm/mod swap drop or the phrase
@ >r m* r> sm/rwm swap drop.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FOLDS_3, "*/", times_div
@ r0    n1
@ r1    n2
@ r2    n3
@ r3    bottom
@ r4    top
    POP_REGS top=r2, to="r0-r1"
    PUSH_TOS
    smull r3, r4, r0, r1;
    cmp r4, #0
    beq 1f                              @ Goto 32bit division
    // TODO 64bit division

@ 32bit division
1:  sdiv tos, r3, r2

@ Return
    bx lr

WORD FLAG_SKIP, "*/mod", times_div_mod
    bx lr

/***************************************************************************//**
@ +
@ ( n1 n2 -- n3 )
@ Add n2 to n1, giving the sum n3
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "+", plus
    ldmia dsp!, {r0}
    adds tos, r0
    bx lr

/***************************************************************************//**
@ +!
@ ( n a-addr -- )
@ Add n to the single-cell number at a-addr.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE, "+!", plus_store
    ldmia dsp!, {r0, r1}
    ldr r2, [tos]
    adds r0, r2
    str r0, [tos]
    movs tos, r1
    bx lr

/***************************************************************************//**
@ +loop
@ ( do-sys -- )
@ Append the run-time semantics given below to the current definition. Resolve
@ the destination of all unresolved occurrences of leave between the location
@ given by do-sys and the next location for a transfer of control, to execute
@ the words following +loop.
@
@ (            n --             )
@ ( R: loop-sys1 -- | loop-sys2 )
@ An ambiguous condition exists if the loop control parameters are unavailable.
@ Add n to the loop index. If the loop index did not cross the boundary between
@ the loop limit minus one and the loop limit, continue execution at the
@ beginning of the loop. Otherwise, discard the current loop control parameters
@ and continue execution immediately following the loop.
 ******************************************************************************/
WORD FLAG_COMPILE_IMMEDIATE, "+loop", plus_loop
    push {lr}

@ pop {r0, r1}
@ adds r0, tos
    ldr r0, =0x1980BC03
    PUSH_REGS r0                        @ ( -- opcode )
    bl comma

@ ldmia dsp!, {tos}
@ cmp r0, r1
    ldr r0, =0x4288CF40
    PUSH_REGS r0                        @ ( -- opcode )
    bl comma

@ it lt
@ pushlt {r0, r1}
    ldr r0, =0xB403BFB8
    PUSH_REGS r0                        @ ( -- opcode )
    bl comma

@ Call branch function
    bl here                             @ ( -- orig )
    SWAP
    bl blt_comma

@ Return
    pop {pc}

/***************************************************************************//**
@ ,
@ ( x -- )
@ Reserve one cell of memory-space and store x in the cell. If the memory-space
@ pointer is aligned when , begins execution, it will remain aligned when ,
@ finishes execution. An ambiguous condition exists if the memory-space pointer
@ is not aligned prior to execution of ,.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE, ",", comma
    ldr r0, =ram_begin
    ldr r1, [r0]
    str tos, [r1], #4                   @ Write x to address in ram_begin
    str r1, [r0]                        @ Update address in ram_begin
    DROP                                @ ( x -- )
    bx lr
.ltorg

/***************************************************************************//**
@ ,?
@ ( -- true | false )
@ Return true if compiler is currently compiling to flash. Return false if
@ compiler is currently compiling to ram.
 ******************************************************************************/
WORD FLAG_SKIP, ",?", comma_q
    PUSH_TOS
    ldr r0, =status_compiler
    ldr r0, [r0]
    cmp r0, #0
    ite ne
    movne tos, #-1
    moveq tos, #0
    bx lr

/***************************************************************************//**
@ ,toflash
@ ( -- )
@ The next definition goes into flash
 ******************************************************************************/
WORD FLAG_INTERPRET, ",toflash", comma_to_flash
    ldr r0, =status_compiler
    movs r1, #-1
    str r1, [r0]
    bx lr

/***************************************************************************//**
@ ,toram
@ ( -- )
@ The next definition goes into ram
 ******************************************************************************/
WORD FLAG_INTERPRET, ",toram", comma_to_ram
    ldr r0, =status_compiler
    movs r1, #0
    str r1, [r0]
    bx lr

/***************************************************************************//**
@ -
@ ( n1 n2 -- n3 )
@ Subtract n2 from n1, giving the difference n3
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "-", minus
    ldmia dsp!, {r0}
    subs tos, r0, tos
    bx lr

/*
WORD FLAG_SKIP, ".", dot
    bx lr
*/

/*
WORD FLAG_SKIP, ".\"", dot_q
    bx lr
*/

/***************************************************************************//**
@ /
@ ( n1 n2 -- n3 )
@ Divide n1 by n2, giving the single-cell quotient n3
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "/", div
    ldmia dsp!, {r0}
    sdiv tos, r0, tos
    bx lr

/*
WORD FLAG_SKIP, "/mod", div_mod
    bx lr
*/

/***************************************************************************//**
@ 0<
@ ( n -- flag )
@ flag is true if and only if n is less than zero.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "0<", zero_less
    cmp tos, #0
    ite lt
    movlt tos, #-1
    movge tos, #0
    bx lr

/***************************************************************************//**
@ 0<
@ ( n -- flag )
@ flag is true if and only if x is equal to zero.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "0=", zero_equal
    cmp tos, #0
    ite eq
    moveq tos, #-1
    movne tos, #0
    bx lr

/***************************************************************************//**
@ 1+
@ ( n1 -- n2 )
@ Add one (1) to n1 giving the sum n2.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "1+", one_plus
    adds tos, #1
    bx lr

/***************************************************************************//**
@ 1-
@ ( n1 -- n2 )
@ Subtract one (1) from n1 giving the difference n2.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "1-", one_minus
    subs tos, #1
    bx lr

/*
WORD FLAG_SKIP, "2!", two_store
    bx lr
*/

/***************************************************************************//**
@ 2*
@ ( x1 -- x2 )
@ x2 is the result of shifting x1 one bit toward the most-significant bit,
@ filling the vacated least-significant bit with zero.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "2*", two_times
    lsl tos, #1
    bx lr

/***************************************************************************//**
@ 2/
@ ( x1 -- x2 )
@ x2 is the result of shifting x1 one bit toward the least-significant bit,
@ leaving the most-significant bit unchanged.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "2/", two_div
    asr tos, #1
    bx lr

/*
WORD FLAG_SKIP, "2@", two_fetch
    bx lr
*/

/***************************************************************************//**
@ 2drop
@ ( x1 x2 -- )
@ Drop cell pair x1 x2 from the stack.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "2drop", two_drop
    TWO_DROP
    bx lr

/***************************************************************************//**
@ 2dup
@ ( x1 x2 -- x1 x2 x1 x2 )
@ Duplicate cell pair x1 x2.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "2dup", two_dup
    TWO_DUP
    bx lr

/***************************************************************************//**
@ 2over
@ ( x1 x2 x3 x4 -- x1 x2 x3 x4 x1 x2 )
@ Copy cell pair x1 x2 to the top of the stack.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE, "2over", two_over
    TWO_OVER
    bx lr

/***************************************************************************//**
@ 2swap
@ ( x1 x2 x3 x4 -- x3 x4 x1 x2 )
@ Exchange the top two cell pairs.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE, "2swap", two_swap
    TWO_SWAP
    bx lr

/***************************************************************************//**
@ :
@ ( source "<spaces>name" -- colon-sys )
@ Skip leading space delimiters. Parse name delimited by a space. Create a
@ definition for name, called a "colon definition". Enter compilation state and
@ start the current definition, producing colon-sys. Append the initiation
@ semantics given below to the current definition.
@
@ The execution semantics of name will be determined by the words compiled into
@ the body of the definition. The current definition shall not be findable in
@ the dictionary until it is ended (or until the execution of DOES> in some
@ systems).
 ******************************************************************************/
WORD FLAG_INTERPRET, ":", colon
    push {lr}

// TODO check stack balance? (maybe even return stack?)

@ Create
    bl create

@ push {lr}
    PUSH_INT16 #0xB500                  @ ( -- opcode )
    bl h_comma                          @ Write opcode

@ Enter compilation state
    bl bracket_right

@ Return
    pop {pc}
.ltorg

/***************************************************************************//**
@ ;
@ ( colon-sys -- )
@ Append the run-time semantics below to the current definition. End the current
@ definition, allow it to be found in the dictionary and enter interpretation
@ state, consuming colon-sys. If the memory-space pointer is not aligned,
@ reserve enough memory-space to align it.
 ******************************************************************************/
WORD FLAG_COMPILE_IMMEDIATE, ";", semicolon
    push {lr}

// TODO check stack balance? (maybe even return stack?)

@ Write return
    bl exit

@ End
    PUSH_INT8 #FLAG_INTERPRET_COMPILE   @ ( -- flags )
    bl end_colon_semicolon

@ Enter interpretation state
    bl bracket_left

@ Return
    pop {pc}

/***************************************************************************//**
@ <
@ ( n1 n2 -- flag )
@ Flag is true if and only if n1 is less than n2.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FOLDS_2, "<", less
    ldmia dsp!, {r0}
    cmp r0, tos
    ite lt
    movlt tos, #-1
    movge tos, #0
    bx lr

/*
WORD FLAG_SKIP, "<#", num_start
    bx lr
*/

/***************************************************************************//**
@ =
@ ( x1 x2 -- flag )
@ Flag is true if and only if x1 is bit-for-bit the same as x2.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FOLDS_2, "=", equal
    ldmia dsp!, {r0}
    cmp r0, tos
    ite eq
    moveq tos, #-1
    movne tos, #0
    bx lr

/***************************************************************************//**
@ >
@ ( n1 n2 -- flag )
@ Flag is true if and only if n1 is greater than n2.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FOLDS_2, ">", more
    ldmia dsp!, {r0}
    cmp r0, tos
    ite gt
    movgt tos, #-1
    movle tos, #0
    bx lr

/*
WORD FLAG_SKIP, ">body", to_body
    bx lr
*/

/***************************************************************************//**
@ >in
@ ( -- a-addr )
@ a-addr is the address of a cell containing the offset in characters from the
@ start of the input buffer to the start of the parse area.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE, ">in", to_in
    PUSH_TOS
    ldr tos, =in
    bx lr

/*
WORD FLAG_SKIP, ">number", to_number
    bx lr
*/

/***************************************************************************//**
@ >r
@ (    x --   )
@ ( R:   -- x )
@ Move x to the return stack.
 ******************************************************************************/
WORD FLAG_COMPILE & FLAG_INLINE, ">r", to_r
    TO_R
    bx lr

/***************************************************************************//**
@ >r
@ ( x -- 0 | x x )
@ Duplicate x if it is non-zero.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE, "?dup", q_dup
    Q_DUP
    bx lr

/***************************************************************************//**
@ @
@ ( a-addr -- x )
@ x is the value stored at a-addr.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE, "@", fetch
    ldr tos, [tos]
    bx lr

/*
WORD FLAG_SKIP, "abort"
    bx lr
*/

/*
WORD FLAG_SKIP, "abort\"", abort_q
    bx lr
*/

/***************************************************************************//**
@ abs
@ ( n -- u )
@ u is the absolute value of n.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "abs"
    asrs r0, tos, #31
    adds tos, r0
    eors tos, r0
    bx lr

/*
WORD FLAG_SKIP, "accept"
    bx lr
*/

/***************************************************************************//**
@ align2
@ ( -- )
@ If the memory-space pointer is not aligned, align it to 2-bytes.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE, "align2"
    ldr r0, =ram_begin
    ldr r1, [r0]
    P2ALIGN1 align=r1, scratch=r12
    str r1, [r0]
    bx lr

/***************************************************************************//**
@ align4
@ ( -- )
@ If the memory-space pointer is not aligned, align it to 4-bytes.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE, "align4"
    ldr r0, =ram_begin
    ldr r1, [r0]
    P2ALIGN2 align=r1, scratch=r12
    str r1, [r0]
    bx lr

/***************************************************************************//**
@ align8
@ ( -- )
@ If the memory-space pointer is not aligned, align it to 8-bytes.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE, "align8"
    ldr r0, =ram_begin
    ldr r1, [r0]
    P2ALIGN3 align=r1, scratch=r12
    str r1, [r0]
    bx lr

/***************************************************************************//**
@ aligned2
@ ( addr -- a-addr )
@ a-addr is the first 2-byte aligned address greater than or equal to addr.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE, "aligned2"
    P2ALIGN1 tos
    bx lr

/***************************************************************************//**
@ aligned2
@ ( addr -- a-addr )
@ a-addr is the first 4-byte aligned address greater than or equal to addr.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE, "aligned4"
    P2ALIGN2 tos
    bx lr

/***************************************************************************//**
@ aligned8
@ ( addr -- a-addr )
@ a-addr is the first 8-byte aligned address greater than or equal to addr.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE, "aligned8"
    P2ALIGN3 tos
    bx lr

/***************************************************************************//**
@ allot
@ ( n -- )
@ If n is greater than zero, reserve n address units of memory-space. If n is
@ less than zero, release | n | address units of memory-space. If n is zero,
@ leave the memory-space pointer unchanged.
@ If the memory-space pointer is aligned and n is a multiple of the size of a
@ cell when allot begins execution, it will remain aligned when allot finishes
@ execution.
@ If the memory-space pointer is character aligned and n is a multiple of the
@ size of a character when allot begins execution, it will remain character
@ aligned when allot finishes execution.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE, "allot"
    ldr r0, =ram_begin
    ldr r1, [r0]
    adds r1, tos
    str r1, [r0]
    DROP                                @ ( n -- )
    bx lr

/***************************************************************************//**
@ and
@ ( x1 x2 -- x3 )
@ x3 is the bit-by-bit logical "and" of x1 with x2.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "and"
    ldmia dsp!, {r0}
    ands tos, r0
    bx lr

/***************************************************************************//**
@ b,
@ ( orig dest -- )
@ Compile an unconditional jump from orig to dest. For future-proofness the
@ 32bit encoding t4 is used as instruction.
 ******************************************************************************/
WORD FLAG_SKIP, "b,", b_comma
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
    ble 1f                              @ Goto temporarily set ram_begin to orig if necessary
        DROP                            @ ( orig -- )
        PRINT "'shi' branch offset too far positive >>>b,<<<"
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
        PUSH_TOS                        @ ( -- ram_begin )

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
@ base
@ ( -- a-addr )
@ a-addr is the address of a cell containing the current number-conversion radix
@ {{2...36}}.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE, "base"
    PUSH_TOS
    ldr tos, =radix                     @ ( -- a-addr )
    bx lr

/***************************************************************************//**
@ begin
@ ( -- dest )
@ Put the next location for a transfer of control, dest, onto the stack. Append
@ the run-time semantics given below to the current definition.
@
@ ( -- )
@ Continue execution.
 ******************************************************************************/
WORD FLAG_COMPILE_IMMEDIATE, "begin"
    push {lr}

@ Destination for branch
    bl here                             @ ( -- dest )

@ Return
    pop {pc}

/***************************************************************************//**
@ beq,
@ ( orig dest -- )
@ Compile a conditional equal jump from orig to dest. For future-proofness the
@ 32bit encoding t3 is used as instruction.
 ******************************************************************************/
WORD FLAG_SKIP, "beq,", beq_comma
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
    ble 1f                              @ Goto temporarily set ram_begin to orig if necessary
        DROP                            @ ( orig -- )
        PRINT "'shi' conditional branch offset too far positive >>>beq,<<<"
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
@ binary
@ ( -- )
@ Set contents of radix to two.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE, "binary"
    ldr r0, =radix
    movs r1, #2
    str r1, [r0]
    bx lr

/*
WORD FLAG_SKIP, "bl"
    bx lr
*/

/***************************************************************************//**
@ compile,
@ ( xt -- )
@ Append the execution semantics of the definition represented by xt to the
@ execution semantics of the current definition.
 ******************************************************************************/
WORD FLAG_COMPILE, "compile,", compile_comma
    push {lr}

@ Ram or flash
    bl comma_q                          @ ( -- true | false )
    cmp tos, #0
    beq 1f                              @ Goto bl, ram
        b 2f                            @ Goto bl, flash

@ bl, ram
@ tos   ram_begin
@ r0    pc-relative address
@ r2    xt
1:  DROP                                @ ( false -- )
    bl here                             @ ( -- ram_begin )
    SWAP                                @ ( xt ram_begin -- ram_begin xt )
    POP_REGS r2                         @ ( xt -- )
    subs r0, r2, tos                    @ xt - ram_begin
    subs r0, #4                         @ pc is 4 bytes ahead in thumb/thumb2!
    b 1f                                @ Goto range check for bl

@ bl, flash
@ tos   xt
@ r0    pc-relative address
@ r2    xt
2:  DROP                                @ ( true -- )
    ldr r0, =ram_begin_def
    ldmia r0, {r1, r2}
    subs r2, r1                         @ Length of current definition
    ldr r0, =flash_begin
    ldr r0, [r0]                        @ Beginning of current definition in flash
    adds r0, r2                         @ Address current definition would have in flash so far
    subs r0, tos, r0                    @ pc-relative address
    subs r0, #4                         @ pc is 4 bytes ahead in thumb/thumb2!
    movs r2, tos                        @ Keep xt in r2 for later use

@ Range check for bl
@ r0    pc-relative address
1:  cmp r0, #-16777216                  @ pc-relative address - -16777216
    blt 3f                              @ Goto movw movt blx

    ldr r1, =16777214
    cmp r0, r1                          @ pc-relative address - 16777214
    bgt 3f                              @ Goto movw movt blx

@ bl
@ tos   opcode
@ r0    pc-relative address (xt - (memory-space pointer + 4))
@ r1    J1 | J2 | imm11 | imm10
    ldr tos, =0xF000D000                @ Opcode template

    cmp r0, #0                          @ pc-relative address - 0
    blt 1f                              @ Goto signed

@ Unsigned
    ands r1, r0, #0x800000              @ J1 = !I1
    it eq
    orreq tos, #0x2000

    ands r1, r0, #0x400000              @ J2 = !I2
    it eq
    orreq tos, #0x800
    b 2f

@ Signed
1:  ands r1, r0, #0x800000              @ J1 = I1
    it ne
    orrne tos, #0x2000

    ands r1, r0, #0x400000              @ J2 = I2
    it ne
    orrne tos, #0x800

    orr tos, #0x4000000                 @ Set sign

2:  lsrs r0, #1
    movw r1, #0x7FF                     @ Mask for imm11
    ands r1, r0                         @ imm11
    orrs tos, r1                        @ Or imm11 into template

    lsrs r0, #11
    movw r1, #0x3FF                     @ Mask for imm10
    ands r1, r0                         @ imm10
    orrs tos, tos, r1, lsl #16          @ Or imm10 into template

    bl rev_comma                        @ Write opcode
        b 6f                            @ Goto return

@ movw movt blx
@ bl coudln't cover our range, do movw movt blx
@ tos   opcode
@ r0    bottom | top
@ r1    intermediate
@ r2    xt + 1
3:  adds r2, #1                         @ Make xt odd (thumb)

@ movw
    ldr tos, =0xF2400000                @ Opcode template

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

    ldr tos, =0xF2C00000                @ Opcode template

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

@ blx r0
    PUSH_INT16 #0x4780                  @ ( -- opcode )
    bl h_comma                          @ Write opcode

@ Return
6:  pop {pc}

/*
WORD FLAG_SKIP, "c!", c_store
    bx lr
*/

/***************************************************************************//**
@ c,
@ ( char -- )
@ Reserve space for one character in the memory-space and store char in the
@ space. If the memory-space pointer is character aligned when c, begins
@ execution, it will remain character aligned when c, finishes execution. An
@ ambiguous condition exists if the memory-space pointer is not
@ character-aligned prior to execution of c,.
 ******************************************************************************/
WORD FLAG_SKIP, "c,", c_comma
    ldr r0, =ram_begin
    ldr r1, [r0]
    strb tos, [r1], #1                  @ Write char to address in ram_begin
    str r1, [r0]                        @ Update address in ram_begin
    DROP                                @ ( char -- )
    bx lr

/***************************************************************************//**
@ c-variable
@ ( source: "<spaces>name" -- )
@ (                 a-addr -- )
@ Skip leading space delimiters. Parse name delimited by a space. Create a
@ definition for name with the execution semantics defined below.
@
@ ( -- a-addr )
@ a-addr is the address of the referenced C variable
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE, "c-variable", c_variable
    push {lr}

@ Create
    bl create

@ Write literal with the C variables address
    bl literal

@ bx lr
    PUSH_INT16 #0x4770
    bl h_comma

@ End
    PUSH_INT8 #FLAG_INTERPRET_COMPILE   @ ( -- flags )
    bl end_colon_semicolon

@ Return
    pop {pc}

/*
WORD FLAG_SKIP, "c@", c_fetch
    bx lr
*/

/***************************************************************************//**
@ cell+
@ ( a-addr1 -- a-addr2 )
@ Add the size in address units of a cell to a-addr1, giving a-addr2.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "cell+", cell_plus
    adds tos, #4
    bx lr

/***************************************************************************//**
@ cells
@ ( n1 -- n2 )
@ n2 is the size in address units of n1 cells.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "cells", cells
    lsl tos, #2
    bx lr

/*
WORD FLAG_SKIP, "char"
    bx lr
*/

/*
WORD FLAG_SKIP, "char+", char_plus
    bx lr
*/

/*
WORD FLAG_SKIP, "chars"
    bx lr
*/

/***************************************************************************//**
@ constant
@ ( source: "<spaces>name" -- )
@ (                      x -- )
@ Skip leading space delimiters. Parse name delimited by a space. Create a
@ definition for name with the execution semantics defined below.
@ name is referred to as a "constant".
@
@ ( -- x )
@ Place x on the stack.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE, "constant"
    push {lr}

@ Create
    bl create

@ Write literal
    bl literal

@ bx lr
    PUSH_INT16 #0x4770
    bl h_comma

@ End
    PUSH_INT8 #FLAG_INTERPRET_COMPILE   @ ( -- flags )
    bl end_colon_semicolon

@ Return
    pop {pc}

/*
WORD FLAG_SKIP, "count"
    bx lr
*/

/*
WORD FLAG_SKIP, "cr"
    bx lr

/***************************************************************************//**
@ create
@ ( source: "<spaces>name" -- )
@ Skip leading space delimiters. Parse name delimited by a space. Create a
@ definition for name with the execution semantics defined below. If the
@ memory-space pointer is not aligned, reserve enough memory-space to align it.
@ The new memory-space pointer defines name's data field. Create does not
@ allocate memory-space in name's data field.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE, "create"
    push {lr}

@ Parse
    bl source                           @ ( -- c-addr u )
    bl parse                            @ ( -- token-addr token-u )
    cmp tos, #0                         @ token-u - 0
    bne 1f                              @ Goto find
        TWO_DROP                        @ ( token-addr false -- )
        PRINT "'shi' attempt to use zero-length string as a name >>>create<<<"
        b 6f                            @ Goto return

@ Find
1:  TWO_DUP                             @ ( -- token-addr token-u token-addr token-u )
    bl find                             @ ( -- token-addr 0 | xt flags )
    // TODO maybe write a "create" which doesn't check for redefinition?
    cmp tos, #0                         @ flags - 0
    beq 1f                              @ Goto create
        TWO_DROP                        @ ( xt flags -- )
        TWO_DROP                        @ ( token-addr token-u -- )
        PRINT "'shi' redefined word >>>create<<<"
        // TODO What should we do in case a word gets redefined?
        // Maybe src should be dropped, so that when we leave create we also
        // leave evaluate?
        b 6f                            @ Goto return

@ Create
@ Mark beginning of new definition
@ r0    ram_begin address
@ r1    ram_begin
1:  ldr r0, =ram_begin
    ldr r1, [r0]
    str r1, [r0, #-4]                   @ Store beginning of new definition in ram_begin_def

@ Take account of link and flags
    TWO_DROP                            @ ( token-addr 0 -- )
    PUSH_INT8 #5
    bl allot

@ Write cstring
@ tos   character
@ r0    token-addr
@ r1    token-addr + token-u
    DUP                                 @ ( -- token-addr token-u token-u)
    bl c_comma
    POP_REGS top=r1, to=r0
    adds r1, r1, r0
1:  cmp r1, r0                          @ token-addr + token-u - token-addr
    bls 1f                              @ Goto align ram_begin
        PUSH_TOS
        ldrb tos, [r0], #1              @ character
        push {r0, r1}
        bl c_comma                      @ Write character
        pop {r0, r1}
        b 1b

@ Align ram_begin
@ Name could have been any length and screw with alignment
1:  bl align2

@ Return
6:  pop {pc}

/***************************************************************************//**
@ decimal
@ ( -- )
@ Set contents of radix to ten.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE, "decimal"
    ldr r0, =radix
    movs r1, #10
    str r1, [r0]
    bx lr

/***************************************************************************//**
@ depth
@ ( -- +n )
@ +n is the number of single-cell values contained in the data stack before +n
@ was placed on the stack.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE, "depth"
    DEPTH
    bx lr

/***************************************************************************//**
@ do
@ ( -- do-sys )
@ Place do-sys onto the stack. Append the run-time semantics given below to the
@ current definition. The semantics are incomplete until resolved by a consumer
@ of do-sys such as loop.
@
@ (    n1 n2 --          )
@ ( R:       -- loop-sys )
@ Set up loop control parameters with index n2 and limit n1. An ambiguous
@ condition exists if n1 and n2 are not both the same type. Anything already on
@ the return stack becomes unavailable until the loop-control parameters are
@ discarded.
 ******************************************************************************/
WORD FLAG_COMPILE_IMMEDIATE, "do"
    push {lr}

@ Do consumes n1 and n2 and pushes it onto the return stack
@ movs r0, tos
@ ldmia dsp!, {r1, tos}
    ldr r0, =0xCF420030
    PUSH_REGS r0                        @ ( -- opcode )
    bl comma

@ push {r0, r1}
    PUSH_INT16 #0xB403                  @ ( -- opcode )
    bl h_comma

@ Do-sys
    bl here                             @ ( -- do-sys )

@ Return
    pop {pc}                          

/***************************************************************************//**
@ does>
@ ( colon-sys1 -- colon-sys2 )
@ Append the run-time semantics below to the current definition. Whether or not
@ the current definition is rendered findable in the dictionary by the
@ compilation of does> is implementation defined. Consume colon-sys1 and produce
@ colon-sys2. Append the initiation semantics given below to the current
@ definition.
@
@ ( -- ) ( R: nest-sys1 -- )
@ Replace the execution semantics of the most recent definition, referred to as
@ name, with the name execution semantics given below. Return control to the
@ calling definition specified by nest-sys1. An ambiguous condition exists if
@ name was not defined with create or a user-defined word that calls create.
 ******************************************************************************/
WORD FLAG_COMPILE, "does>", does
    nop
    nop
    nop

    // https://softwareengineering.stackexchange.com/questions/339283/forth-how-do-create-and-does-work-exactly
    // oida -.-

    @ Hmm... ich fürcht wir müssen create auch nochmal umschreiben
    @ Folgendes Beispiel geht aktuell nicht:
    @ "17 create seventeen ,"
    @ und zwar deshalb, weil ohne ; keine Links und Flags geschrieben werden
    @ das heißt der Eintrag wird zwar erzeugt und 17 in Speicher gschrieben
    @ auffindbar is der Eintrag aber nie -.-

    @ Folgende Idee find ich eigentlich nicht so schlecht:
    @ In LR steht aktuell eine Rücksprungadresse, die auf den Code hinter does> zeigt.
    @ Also eigentlich genau dorthin wo ma hinwollen.
    @ Es erscheint also irgendwie logisch in die aktuelle Definition einfach:
    @ PUSH_REGS lr
    @ bl compile_comma
    @ zu schreiben um eine Branch zum Code des define words zu erzeugen und an die Stelle
    @ hinter does> zu springen...

    @ Das Problem is nur, dass das bei dem kanonischen Beispiel
    @   ": my_constant create , does> @ ;"_fs;
    @   "7 my_constant CON"_fs;
    @ ein Wort "CON" erzeugt wird, dass bei der Ausführung zuerst 7 im Speicher stehn hat
    @ und dann eine Branch zum FETCH Teil von "my_constant"...

    @ Da fallen also 2 Fragen an:
    @ 1.) Muss does> die Adresse vom Datenfeld wo 7 steht in die Definition schreiben bevor gebranched wird?
    @ 2.) Wie verhindert ma dass ma beim Aufruf von CON auf 7 hupft statt dorthin wo does hinzeigt?

    @ bl, zum LR in die aktuelle definition screiben?
    @PUSH_REGS lr
    @bl compile_comma
    @nop

    @ und an exit brauchts vielleicht a?
    @ um die aktuelle definition abzuschließen
    @ oder glei semicolon...?
    @bl semicolon

    @ directly return to the caller...
    pop {pc}

/***************************************************************************//**
@ drop
@ ( x -- )
@ Remove x from the stack.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "drop"
    DROP
    bx lr

/***************************************************************************//**
@ dup
@ ( x -- x x )
@ Duplicate x.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "dup"
    DUP
    bx lr

/***************************************************************************//**
@ else
@ ( orig1 fp-to-branch1 -- orig2 fp-to-branch2 )
@ Put the location of a new unresolved forward reference orig2 and it's function
@ pointer onto the stack. Append the run-time semantics given below to the
@ current definition. The semantics will be incomplete until orig2 is resolved
@ (e.g., by then). Resolve the forward reference orig1 using the location
@ following the appended run-time semantics.
@
@ ( -- )
@ Continue execution at the location given by the resolution of orig2.
 ******************************************************************************/
WORD FLAG_COMPILE_IMMEDIATE, "else"
    push {lr}

@ Reserve space for orig2
    bl here                             @ ( -- orig2)
    PUSH_INT8 #4
    bl allot

@ Resolve branch from pointer from if or else
@ tos   dest
@ r0    fp-to-branch1
@ r1    dest
@ r2    orig1
@ r3    fp-to-branch2
@ r4    orig2
    POP_REGS top=r4, to="r0,r2"         @ (orig1 fp-to-branch1 orig2 -- )
    adds r1, r4, #4                     @ dest = orig2 + 4
    ldr r3, =b_comma
    PUSH_REGS top=r1, from="r2-r4"      @ ( -- orig2 fp-to-branch2 orig1 dest )
    blx r0

@ Return
    pop {pc}

/*
WORD FLAG_SKIP, "emit"
    bx lr
*/

/***************************************************************************//**
@ end:;
@ TODO
@ ANS forces us to not write flags before the definition is completed!
@ So write them now...
 ******************************************************************************/
WORD FLAG_SKIP, "end:;", end_colon_semicolon
    push {lr}

@ Ram or flash
    bl comma_q                          @ ( -- true | false )
    cmp tos, #0
    beq 1f                              @ Goto end:; ram
        b 2f                            @ Goto end:; flash

@ end:; ram
@ tos   flags
@ r0    ram_begin_def address
@ r1    ram_begin_def
@ r2    ram_begin
@ r3    link address
@ r4    link
1:  DROP                                @ ( false -- )
    ldr r0, =ram_begin_def
    ldmia r0, {r1, r2}
    ldr r3, =link
    ldr r4, [r3]
    str r1, [r3]                        @ Update last link
    str r4, [r1], #4                    @ Write link
    strb tos, [r1]                      @ Write flags
    b 6f                                @ Goto return

@ end:; flash
@ tos   flags
@ r0    ram_begin_def
@ r1    ram_begin
@ r2    flash_begin
2:  DROP                                @ ( false -- )
    ldr r2, =ram_begin_def
    ldmia r2, {r0, r1}
    strb tos, [r0, #4]!                 @ Write flags
    ldr r2, =flash_begin
    ldr r2, [r2]
    bl shi_write_flash
    ldr r1, =flash_begin                @ Update flash_begin
    str r0, [r1]

@ Clear definition from ram
@ r0    ram_begin_def address
@ r1    ram_begin_def
@ r2    ram_begin
@ r3    erased word
    ldr r0, =ram_begin_def
    ldmia r0, {r1, r2}
    P2ALIGN2 align=r2, scratch=r3
    movs r3, #ERASED_WORD
2:  str r3, [r2], #-4
    cmp r1, r2
    blo 2b
    stmia r0, {r1, r2}

@ Return
6:  DROP                                @ ( flags -- )
    pop {pc}

/*
WORD FLAG_SKIP, "environment?", environment_q
    bx lr
*/

/***************************************************************************//**
@ evaluate
@ ( i*x c-addr u -- j*x )
@ Save the current input source specification. Store minus-one (-1) in SOURCE-ID
@ if it is present. Make the string described by c-addr and u both the input
@ source and input buffer, set >IN to zero, and interpret. When the parse area
@ is empty, restore the prior input source specification. Other stack effects
@ are due to the words EVALUATEd.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE, "evaluate"
    push {lr}

@ Store source
@ tos   u
@ r0    c-addr
@ r1    src address
    ldr r0, [dsp]
    ldr r1, =src
    stmia r1, {r0, tos}
    TWO_DROP                            @ ( c-addr u -- )

@ Set >IN 0
    SET_IN #0                           @ Set >in zero

@ Interpret
    bl interpret                        @ Start interpreter

@ Return
    pop {pc}

/***************************************************************************//**
@ execute
@ ( i * x xt -- j * x )
@ Remove xt from the stack and perform the semantics identified by it. Other
@ stack effects are due to the word executed.
 ******************************************************************************/
WORD FLAG_INTERPRET, "execute"
    POP_REGS r0
    mov pc, r0

/***************************************************************************//**
@ exit
@ (             -- )
@ ( R: nest-sys -- )
@ Return control to the calling definition specified by nest-sys. Before
@ executing exit within a do-loop, a program shall discard the loop-control
@ parameters by executing unloop.
 ******************************************************************************/
WORD FLAG_COMPILE_IMMEDIATE, "exit"
    push {lr}

@ pop {pc}
    PUSH_INT16 #0xBD00                  @ ( -- opcode )
    bl h_comma                          @ Write opcode

@ Return
    pop {pc}

/*
WORD FLAG_SKIP, "fill"
    bx lr
*/

/***************************************************************************//**
@ find
@ ( token-addr token-u -- token-addr false | xt flags )
@ Find the definition named in the counted string at c-addr. If the definition
@ is not found, return token-addr and zero. If the definition is found, return
@ its execution token xt and its flags.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE, "find"
@ tos   token-addr
@ r0    token-u
@ r1    link
@ r2    flags
@ r3    word-u
@ r4    c-addr incremented
@ r5    token-addr incremented
@ r9    character
@ r12   character
    ldr r1, =link                       @ Begin search at latest link
    POP_REGS r0                         @ ( token-u -- )

@ Search
1:  ldr r1, [r1]                        @ Link
    cmp r1, #LINK_INVALID               @ link - LINK_INVALID
    beq 4f                              @ Goto found nothing
        ldrb r2, [r1, #4]               @ Flags
        cmp r2, #FLAG_SKIP              @ Check if word should be skipped
        beq 1b                          @ Goto search

@ String compare
    ldrb r3, [r1, #5]                   @ u
    cmp r0, r3                          @ token-u - (c-addr incremented)
    bne 1b                              @ Goto search
        adds r4, r1, #6                 @ c-addr
        movs r5, tos                    @ token-addr
        cmp r3, #4                      @ token-u - 4
        bhs 3f                          @ Goto wordwise compare

@ Bytewise compare
2:      ldrb r9, [r4], #1               @ Character
        ldrb r12, [r5], #1
        cmp r9, r12                     @ character - character
        bne 1b                          @ Goto search
            subs r3, #1
            cmp r3, #0
            bhi 2b                      @ Goto bytewise compare
                b 5f                    @ Goto found something

@ Wordwise compare
3:      ldr r9, [r4], #4                @ 4x Characters
        ldr r12, [r5], #4               @ 4x Characters
        cmp r9, r12                     @ character - character
        bne 1b
            subs r3, #4
            cmp r3, #4                  @ token-u - 4
            bhs 3b                      @ Goto wordwise compare
                cmp r3, #0              @ token-u - 0
                bhi 2b                  @ Goto bytewise compare
                    b 5f                @ Goto found something

@ Found nothing
4:  PUSH_INT8 #0                        @ ( -- 0 )
    b 6f                                @ Goto return

@ Found something
@ Only keep tos, r2 and r4
5:  P2ALIGN1 r4                         @ xt is at next 2-byte aligned address
    DROP                                @ ( token-addr -- )
    PUSH_REGS top=r2, from=r4           @ ( -- xt flags )

@ Return
6:  bx lr

/*
WORD FLAG_SKIP, "fm/mod", fm_div_mod
    bx lr
*/

/***************************************************************************//**
@ h,
@ ( h -- )
@ Reserve half a cell of memory-space and store x in that place. If the
@ memory-space pointer is aligned when h, begins execution, it will remain
@ aligned when h, finishes execution. An ambiguous condition exists if the
@ memory-space pointer is not aligned prior to execution of h,.
 ******************************************************************************/
WORD FLAG_SKIP, "h,", h_comma
    ldr r0, =ram_begin
    ldr r1, [r0]
    strh tos, [r1], #2                  @ Write h to address in ram_begin
    str r1, [r0]                        @ Update address in ram_begin
    DROP                                @ ( h -- )
    bx lr

/***************************************************************************//**
@ here
@ ( -- addr )
@ addr is the memory-space pointer.
 ******************************************************************************/
WORD FLAG_SKIP, "here"
    PUSH_TOS
    ldr tos, =ram_begin
    ldr tos, [tos]
    bx lr

/*
WORD FLAG_SKIP, "hold"
    bx lr
*/

/***************************************************************************//**
@ i
@ (             -- n        )
@ ( R: loop-sys -- loop-sys )
@ n is a copy of the current (innermost) loop index. An ambiguous condition
@ exists if the loop control parameters are unavailable.
 ******************************************************************************/
WORD FLAG_COMPILE_IMMEDIATE, "i"
    push {lr}

@ str tos, [dsp, #-4]!
    ldr r0, =0x6D04F847
    PUSH_REGS r0
    bl comma

@ ldr tos, [sp]
    PUSH_INT16 #0x9E00
    bl h_comma

@ Return
    pop {pc}

/***************************************************************************//**
@ if
@ ( -- orig fp-to-branch )
@ Put the location of a new unresolved forward reference orig and it's function
@ pointer onto the stack. Append the run-time semantics given below to the
@ current definition. The semantics are incomplete until orig is resolved, e.g.,
@ by then or else.
@
@ ( x -- )
@ If all bits of x are zero, continue execution at the location specified by the
@ resolution of orig.
 ******************************************************************************/
WORD FLAG_COMPILE_IMMEDIATE, "if"
    push {lr}

@ If needs to consume its own flag
@ movs r0, tos
@ ldmia dsp!, {tos}
    ldr r0, =0xCF400030
    PUSH_REGS r0                        @ ( -- opcode )
    bl comma

@ cmp r0, #0
    PUSH_INT16 #0x2800                  @ ( -- opcode )
    bl h_comma

@ Orig and pointer to branch function
    bl here                             @ ( -- orig )
    ldr r0, =beq_comma
    PUSH_REGS r0                        @ ( -- fp-to-branch)

@ Reserve space for orig
    PUSH_INT8 #4                        @ Reserve space for branch instruction
    bl allot

@ Return
    pop {pc}
.ltorg

/*
WORD FLAG_SKIP, "immediate"
    bx lr
*/

/***************************************************************************//**
@ invert
@ ( x1 -- x2 )
@ Invert all bits of x1, giving its logical inverse x2.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "invert"
    mvns tos, tos
    bx lr

/***************************************************************************//**
@ j
@ (                        -- n                   )
@ ( R: loop-sys1 loop-sys2 -- loop-sys1 loop-sys2 )
@ n is a copy of the next-outer loop index. An ambiguous condition exists if the
@ loop control parameters of the next-outer loop, loop-sys1, are unavailable.
 ******************************************************************************/
WORD FLAG_COMPILE_IMMEDIATE, "j"
    push {lr}

@ str tos, [dsp, #-4]!
    ldr r0, =0x6D04F847
    PUSH_REGS r0
    bl comma

@ ldr tos, [sp, #8]
    PUSH_INT16 #0x9E02
    bl h_comma

@ Return
    pop {pc}

/*
WORD FLAG_SKIP, "key"
    bx lr
*/

/***************************************************************************//**
@ leave
@ (             -- )
@ ( R: loop-sys -- )
@ Discard the current loop control parameters. An ambiguous condition exists if
@ they are unavailable. Continue execution immediately following the innermost
@ syntactically enclosing do...loop or do...+loop.
 ******************************************************************************/
WORD FLAG_COMPILE_IMMEDIATE, "leave"
    push {lr}

@ Unloop
    bl unloop

@ Orig
    bl here                             @ ( -- orig )

@ Reserve space for orig
    PUSH_INT8 #4                        @ Reserve space for branch instruction
    bl allot

@ Reverse push orig onto the stack
@ r0    csp address
@ r1    csp
    ldr r0, =csp
    ldr r1, [r0]
    stmia r1!, {tos}
    str r1, [r0]
    DROP                                @ ( orig -- )

@ Return
    pop {pc}

/***************************************************************************//**
@ loop
@ ( do-sys -- )
@ Append the run-time semantics given below to the current definition. Resolve
@ the destination of all unresolved occurrences of leave between the location
@ given by do-sys and the next location for a transfer of control, to execute
@ the words following the loop.
@
@ (              --             )
@ ( R: loop-sys1 -- | loop-sys2 )
@ An ambiguous condition exists if the loop control parameters are unavailable.
@ Add one to the loop index. If the loop index is then equal to the loop limit,
@ discard the loop parameters and continue execution immediately following the
@ loop. Otherwise continue execution at the beginning of the loop.
 ******************************************************************************/
WORD FLAG_COMPILE_IMMEDIATE, "loop"
    push {lr}

@ pop {r0, r1}
@ adds r0, #1
    ldr r0, =0x3001BC03
    PUSH_REGS r0                        @ ( -- opcode )
    bl comma

@ cmp r0, r1
@ it lt
    ldr r0, =0xBFB84288
    PUSH_REGS r0                        @ ( -- opcode )
    bl comma

@ pushlt {r0, r1}
    PUSH_INT16 #0xB403                  @ ( -- opcode )
    bl h_comma

@ Call branch function
    bl here                             @ ( -- orig )
    SWAP
    bl blt_comma

@ Leave
@ r0    csp address
@ r1    csp
@ r2    stack address
    ldr r0, =csp
    ldr r1, [r0]
    ldr r2, =_s_shi_dstack
    cmp r1, r2
    beq 6f

@ Check if csp and dsp clash
@ r1    csp
    cmp r1, dsp
    blo 1f
        PRINT "'shi' stack overflow >>>leave<<<"
        b 6f

@ Take care of leave(s)
@ r0    csp address
@ r1    csp
@ r2    orig
@ r3    scratch
1:  ldr r2, [r1, #-4]
    PUSH_REGS r2                        @ ( -- orig )
    movs r3, #0
    str r3, [r1, #-4]!
    str r1, [r0]
    bl here                             @ ( -- dest )
    bl b_comma

@ Return
6:  pop {pc}

/***************************************************************************//**
@ lshift
@ ( x1 u -- x2 )
@ Perform a logical left shift of u bit-places on x1, giving x2. Put zeroes into
@ the least significant bits vacated by the shift. An ambiguous condition exists
@ if u is greater than or equal to the number of bits in a cell.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "lshift"
    ldmia dsp!, {r0}
    lsls tos, r0, tos
    bx lr

/*
WORD FLAG_SKIP, "m*", m_times
    bx lr
*/

/***************************************************************************//**
@ max
@ ( n1 n2 -- n3 )
@ n3 is the greater of n1 and n2.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FOLDS_2, "max"
    ldmia dsp!, {r0}
    cmp r0, tos
    it gt
    movgt tos, r0
    bx lr

/***************************************************************************//**
@ min
@ ( n1 n2 -- n3 )
@ n3 is the lesser of n1 and n2.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FOLDS_2, "min"
    ldmia dsp!, {r0}
    cmp r0, tos
    it lt
    movlt tos, r0
    bx lr

/*
WORD FLAG_SKIP, "mod"
    bx lr
*/

/*
WORD FLAG_SKIP, "move"
    bx lr
*/

/***************************************************************************//**
@ negate
@ ( n1 -- n2 )
@ n2 is the negated value of n1.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "negate"
    rsbs tos, tos, #0
    bx lr

/***************************************************************************//**
@ or
@ ( x1 x2 -- x3 )
@ x3 is the bit-by-bit inclusive-or of x1 with x2.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "or"
    ldmia dsp!, {r0}
    orrs tos, r0
    bx lr

/***************************************************************************//**
@ over
@ ( x1 x2 -- x1 x2 x1 )
@ Place a copy of x1 on top of the stack.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "over"
    OVER
    bx lr

/*
WORD FLAG_SKIP, "postpone"
    bx lr
*/

/*
WORD FLAG_SKIP, "quit"
    bx lr
*/

/***************************************************************************//**
@ >r
@ (      -- x )
@ ( R: x --   )
@ Move x from the return stack to the data-stack.
 ******************************************************************************/
WORD FLAG_COMPILE & FLAG_INLINE, "r>", r_from
    R_FROM
    bx lr

/***************************************************************************//**
@ r@
@ (      -- x )
@ ( R: x -- x )
@ Copy x from the return stack to the data stack.
 ******************************************************************************/
WORD FLAG_COMPILE & FLAG_INLINE, "r@", r_fetch
    PUSH_TOS
    ldr tos, [sp]
    bx lr

/*
WORD FLAG_SKIP, "recurse"
    bx lr
*/

/***************************************************************************//**
@ repeat
@ ( orig dest -- )
@ Append the run-time semantics given below to the current definition, resolving
@ the backward reference dest. Resolve the forward reference orig using the
@ location following the appended run-time semantics.
@
@ ( -- )
@ Continue execution at the location given by dest.
 ******************************************************************************/
WORD FLAG_COMPILE_IMMEDIATE, "repeat"
    push {lr}

@ Resolve branch to begin
    bl here                             @ ( -- orig )
    SWAP
    bl b_comma

@ Resolve branch from while
    bl here                             @ ( -- dest )
    bl beq_comma

@ Return
    pop {pc}

/***************************************************************************//**
@ rev,
@ ( x -- )
@ Reserve one cell of memory-space and store x in reverse order in the cell. If
@ the memory-space pointer is aligned when r, begins execution, it will remain
@ aligned when r, finishes execution. An ambiguous condition exists if the
@ memory-space pointer is not aligned prior to execution of r,.
 ******************************************************************************/
WORD FLAG_SKIP, "rev,", rev_comma
    ldr r0, =ram_begin
    ldr r1, [r0]
    movs r2, tos
    lsrs tos, #16
    strh tos, [r1], #2                  @ Write x to address in ram_begin
    strh r2, [r1], #2                   @ Write x to address in ram_begin
    str r1, [r0]                        @ Update address in ram_begin
    DROP                                @ ( x -- )
    bx lr

/***************************************************************************//**
@ rot
@ ( x1 x2 x3 -- x2 x3 x1 )
@ Rotate the top three stack entries.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE, "rot"
    ROT
    bx lr

/***************************************************************************//**
@ rshift
@ ( x1 u -- x2 )
@ Perform a logical right shift of u bit-places on x1, giving x2. Put zeroes
@ into the most significant bits vacated by the shift. An ambiguous condition
@ exists if u is greater than or equal to the number of bits in a cell.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "rshift"
    ldmia dsp!, {r0}
    lsrs tos, r0, tos
    bx lr

/*
WORD FLAG_SKIP, "s\"", s_q
    bx lr
*/

/*
WORD FLAG_SKIP, "s>d", s_to_d
    bx lr
*/

/*
WORD FLAG_SKIP, "sign"
    bx lr
*/

/*
WORD FLAG_SKIP, "sm/rem", sm_div_rem
    bx lr
*/

/***************************************************************************//**
@ source
@ ( -- c-addr u )
@ c-addr is the address of, and u is the number of characters in, the input
@ buffer.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE, "source"
    ldr r0, =src
    ldmia r0, {r1, r2}
    PUSH_REGS top=r2, from=r1           @ ( -- c-addr u )
    bx lr

/*
WORD FLAG_SKIP, "space"
    bx lr
*/

/*
WORD FLAG_SKIP, "spaces"
    bx lr
*/

/***************************************************************************//**
@ state
@ ( -- a-addr )
@ a-addr is the address of a cell containing the compilation-state flag. State
@ is true when in compilation state, false otherwise. The true value in state is
@ non-zero, but is otherwise implementation-defined. Only the following standard
@ words alter the value in state: : (colon), ; (semicolon), abort, quit,
@ :noname, [ (left-bracket), ] (right-bracket).
 ******************************************************************************/
WORD FLAG_SKIP, "state"
    PUSH_TOS
    ldr tos, =status
    bx lr

/***************************************************************************//**
@ swap
@ ( x1 x2 -- x2 x1 )
@ Exchange the top two stack items.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE, "swap"
    SWAP
    bx lr

/***************************************************************************//**
@ then
@ ( orig fp-to-branch -- )
@ Append the run-time semantics given below to the current definition. Resolve
@ the forward reference orig using the location of the appended run-time
@ semantics.
@
@ ( -- )
@ Continue execution.
 ******************************************************************************/
WORD FLAG_COMPILE_IMMEDIATE, "then"
    push {lr}

@ Destination for branch
    bl here                             @ ( -- dest )

@ Resolve branch from pointer from if or else
@ r0    fp-to-branch
@ r1    dest
    POP_REGS top=r1, to=r0              @ ( orig fp-to-branch dest -- )
    PUSH_REGS r1
    blx r0

@ Return
    pop {pc}

/*
WORD FLAG_SKIP, "type"
    bx lr
*/

/*
WORD FLAG_SKIP, "u.", u_dot
    bx lr
*/

/***************************************************************************//**
@ u<
@ ( u1 u2 -- flag )
@ flag is true if and only if u1 is less than u2.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FOLDS_2, "u<", u_less
    ldmia dsp!, {r0}
    cmp r0, tos
    ite lo
    movlo tos, #-1
    movhs tos, #0
    bx lr

/*
WORD FLAG_SKIP, "um*", um_times
    bx lr
*/

/*
WORD FLAG_SKIP, "um/mod", um_div_mod
    bx lr
*/

/***************************************************************************//**
@ unloop
@ (             -- )
@ ( R: loop-sys -- )
@ Discard the loop-control parameters for the current nesting level. An unloop
@ is required for each nesting level before the definition may be exited. An
@ ambiguous condition exists if the loop-control parameters are unavailable.
 ******************************************************************************/
WORD FLAG_COMPILE_IMMEDIATE, "unloop"
    push {lr}

@ pop {r0, r1}
    PUSH_INT16 #0xBC03
    bl h_comma

@ Return
    pop {pc}

/***************************************************************************//**
@ until
@ ( dest -- )
@ Append the run-time semantics given below to the current definition, resolving
@ the backward reference dest.
@
@ ( x -- )
@ If all bits of x are zero, continue execution at the location specified by
@ dest.
 ******************************************************************************/
WORD FLAG_COMPILE_IMMEDIATE, "until"
    push {lr}

@ Until needs to consume its own flag
@ movs r0, tos
@ ldmia dsp!, {tos}
    ldr r0, =0xCF400030
    PUSH_REGS r0                        @ ( -- opcode )
    bl comma

@ cmp r0, #0
    PUSH_INT16 #0x2800                  @ ( -- opcode )
    bl h_comma

@ Resolve branch to begin
    bl here                             @ ( -- orig )
    SWAP
    bl beq_comma

@ Return
    pop {pc}

/***************************************************************************//**
@ variable
@ ( source: "<spaces>name" -- )
@ Skip leading space delimiters. Parse name delimited by a space. Create a
@ definition for name with the execution semantics defined below. Reserve one
@ cell of data space at an aligned address. The cell gets zero initialized.
@ name is referred to as a "variable".
@
@ ( -- a-addr )
@ a-addr is the address of the reserved cell.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE, "variable"
    push {lr}

@ Create
    bl create

@ Write literal with the reserved cells address
    ldr r0, =ram_end
    ldr r1, [r0]
    movs r2, #0                         @ Zero initialize cell
    str r2, [r1, #-4]!
    PUSH_REGS r1
    str r1, [r0]
    bl literal

@ bx lr
    PUSH_INT16 #0x4770
    bl h_comma

@ End
    PUSH_INT8 #FLAG_INTERPRET_COMPILE & RESERVE_1CELL   @ ( -- flags )
    bl end_colon_semicolon

@ Return
    pop {pc}

/***************************************************************************//**
@ while
@ ( dest -- orig dest )
@ Put the location of a new unresolved forward reference orig onto the stack,
@ under the existing dest. Append the run-time semantics given below to the
@ current definition. The semantics are incomplete until orig and dest are
@ resolved (e.g., by repeat).
@
@ ( x -- )
@ If all bits of x are zero, continue execution at the location specified by the
@ resolution of orig.
 ******************************************************************************/
WORD FLAG_COMPILE_IMMEDIATE, "while"
    push {lr}

@ While needs to consume its own flag
@ movs r0, tos
@ ldmia dsp!, {tos}
    ldr r0, =0xCF400030
    PUSH_REGS r0                        @ ( -- opcode )
    bl comma

@ cmp r0, #0
    PUSH_INT16 #0x2800                  @ ( -- opcode )
    bl h_comma

@ Reserve space for orig
    bl here                             @ ( -- orig )
    SWAP
    PUSH_INT8 #4                        @ Reserve space for branch instruction
    bl allot

@ Return
    pop {pc}

/*
WORD FLAG_SKIP, "word"
    bx lr
*/

/***************************************************************************//**
@ xor
@ ( x1 x2 -- x3 )
@ x3 is the bit-by-bit exclusive-or of x1 with x2.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "xor"
    ldmia dsp!, {r0}
    eors tos, r0
    bx lr

/***************************************************************************//**
@ [
@ ( -- )
@ Enter interpretation state. [ is an immediate word.
 ******************************************************************************/
WORD FLAG_COMPILE_IMMEDIATE, "[", bracket_left
    ldr r0, =status
    movs r1, #0
    str r1, [r0]
    bx lr

/***************************************************************************//**
@ [']
@ ( source: "<spaces>name" --    )
@ Skip leading space delimiters. Parse name delimited by a space. Find name.
@ Append the run-time semantics given below to the current definition.
@ An ambiguous condition exists if name is not found.
@
@ ( -- xt )
@ Place name's execution token xt on the stack. The execution token returned by
@ the compiled phrase "['] x" is the same value returned by "' x" outside of
@ compilation state.
 ******************************************************************************/
WORD FLAG_COMPILE_IMMEDIATE, "[']", bracket_tick
    push {lr}
    bl tick                             @ ( -- xt )
    bl literal                          @ ( xt -- )
    pop {pc}

/*
WORD FLAG_SKIP, "[char]", bracket_char
    bx lr
*/

/***************************************************************************//**
@ [
@ ( -- )
@ Enter compilation state.
 ******************************************************************************/
WORD FLAG_INTERPRET, "]", bracket_right
    ldr r0, =status
    movs r1, #-1
    str r1, [r0]
    bx lr
