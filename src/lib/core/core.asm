/***************************************************************************//**
 *  @brief      Core word set
 *
 *  @file       core.asm
 *  @version    0.1
 *  @author     Vincent Hamp
 *  @date       27/07/2016
 ******************************************************************************/

@ C/C++ interface
.global _s_shi_dstack
.global _e_shi_dstack
.global _s_shi_context

.section .data

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ Core variables @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/***************************************************************************//**
@ Stack
 ******************************************************************************/
    .p2align 2                          @ Make sure the stack is 4-byte aligned (ldm and stm operations need 4-byte alignment)
_s_shi_dstack:                          @ Data-stack start
    .space DSTACK_SIZE                  @ Data-stack size in bytes
_e_shi_dstack:                          @ Data-stack end

    .p2align 2
_s_shi_context:                         @ Used to store the forth context
    .space 12                           @ Save tos, dsp and lfp
_e_forth_context:

/***************************************************************************//**
@ Memory-space pointers
 ******************************************************************************/
     .p2align 2
p_mem_ram:                              @ Pointer to ram
    .word 0
    .word 0                             @ Beginning of current definition

p_mem_flash:                            @ Pointer to flash
    .word 0

p_variable:                             @ Used for reserving ram for variables
    .word 0

p_structure:                            @ Inside loop: points to leave addresses from the current loop on the stack
    .word _s_shi_dstack                 @ Inside case: points to endof addresses from the current case on the stack

/***************************************************************************//**
@ Contains address of link of the last definition
@ l_mem has to point to the first definition in flash, so that the very first
@ definition that gets created in ram has an actual link back to flash.
 ******************************************************************************/
l_mem:                                  @ Last link
    .word _s_shi_dict

/***************************************************************************//**
@ Core variables
 ******************************************************************************/
status:                                 @ Current state (state is taken as word)
    .word 0                             @ false: interpret, true: compile

status_compiler:                        @ Current compiler state
    .word 0                             @ false: compile to ram, true: compile to flash

    .p2align 2                          @ Make sure src is 4-byte aligned
src:                                    @ Source
    .word 0                             @ c-addr
    .word 0                             @ u

in:                                     @ Index in terminal input buffer
    .word 0

radix:                                  @ Determine current numerical base (base is taken as word)
    .word 10

.section .text

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ Core words @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
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

@ Parse ------------------------------------------------------------------------
    bl source                           @ ( -- c-addr u )
    bl parse                            @ ( -- token-addr token-u )
    cmp tos, #0                         @ token-u - 0
    bne 1f                              @ Goto find
        TWO_DROP                        @ ( token-addr false -- )
        TRACE_WRITE "'shi' attempt to use zero-length string as a name >>>'<<<"
        b 6f                            @ Goto return

@ Find -------------------------------------------------------------------------
1:  bl find                             @ ( -- token-addr 0 | xt flags )
    cmp tos, #0                         @ flags - 0
    bne 1f                              @ Goto xt
        TWO_DROP                        @ ( token-addr 0 -- )
        TRACE_WRITE "'shi' undefined word >>>'<<<"
        b 6f                            @ Goto return

@ xt ---------------------------------------------------------------------------
1:  DROP                                @ ( flags -- )

@ Return -----------------------------------------------------------------------
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

@ 32bit division ---------------------------------------------------------------
1:  sdiv tos, r3, r2

@ Return -----------------------------------------------------------------------
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

@ itt lt
@ pushlt {r0, r1}
    ldr r0, =0xB403BFBC
    PUSH_REGS r0                        @ ( -- opcode )
    bl comma

@ Call branch function ---------------------------------------------------------
    bl here                             @ ( -- orig )
    SWAP
    bl blt_comma

@ Return -----------------------------------------------------------------------
    pop {pc}

/***************************************************************************//**
@ ,
@ ( x -- )
@ Reserve one cell of memory-space and store x in the cell. If the memory-space
@ pointer is aligned when , begins execution, it will remain aligned when ,
@ finishes execution. An ambiguous condition exists if the memory-space pointer
@ is not aligned prior to execution of ,.
 ******************************************************************************/
WORD FLAG_SKIP, ",", comma
    ldr r0, =p_mem_ram
    ldr r1, [r0]
    str tos, [r1], #4                   @ Write x to address in p_mem_ram
    str r1, [r0]                        @ Update address in p_mem_ram
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

/*
WORD FLAG_SKIP, "2*", two_times
    bx lr
*/

/*
WORD FLAG_SKIP, "2/", two_div
    bx lr
*/

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

@ Create -----------------------------------------------------------------------
    bl create

@ push {lr}
    PUSH_INT16 #0xB500                  @ ( -- opcode )
    bl h_comma                          @ Write opcode

@ Enter compilation state ------------------------------------------------------
    bl bracket_right

@ Return -----------------------------------------------------------------------
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

@ Write return -----------------------------------------------------------------
    bl exit

@ End --------------------------------------------------------------------------
    PUSH_INT8 #FLAG_INTERPRET_COMPILE   @ ( -- flags )
    bl end_colon_semicolon

@ Enter interpretation state ---------------------------------------------------
    bl bracket_left

@ Return -----------------------------------------------------------------------
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
    ldr r0, =p_mem_ram
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
    ldr r0, =p_mem_ram
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
    ldr r0, =p_mem_ram
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
    ldr r0, =p_mem_ram
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

@ Range check for b ------------------------------------------------------------
@ r1    pc-relative address (dest - (orig + 4))
    cmp r1, #-16777216                  @ pc-relative address - -16777216
    bge 1f
        DROP                            @ ( orig -- )
        TRACE_WRITE "'shi' branch offset too far negative >>>b,<<<"
        b 6f                            @ Goto return

1:  ldr r2, =16777214
    cmp r1, r2                          @ pc-relative address - 16777214
    ble 1f                              @ Goto temporarily set p_mem_ram to orig if necessary
        DROP                            @ ( orig -- )
        TRACE_WRITE "'shi' branch offset too far positive >>>b,<<<"
        b 6f                            @ Goto return

@ Temporarily set p_mem_ram to orig if necessary -------------------------------
@ tos   orig
@ r0    dest
@ r2    p_mem_ram address
@ r3    p_mem_ram
@ r4    flag to indicate whether p_mem_ram is overwritten or not
1:  movs r4, #0                         @ Reset flag
    ldr r2, =p_mem_ram
    ldr r3, [r2]
    cmp tos, r3
    bhs 1f
        movs r4, #1                     @ Set flag
        str tos, [r2]                   @ Temporarily store orig as p_mem_ram
        movs tos, r3
        PUSH_TOS                        @ ( -- p_mem_ram )

@ b ----------------------------------------------------------------------------
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

@ Write opcode, do not reset p_mem_ram -----------------------------------------
@ r4    flag to indicate whether p_mem_ram is overwritten or not
    cmp r4, #0
    bne 1f
        bl rev_comma                    @ Write opcode
        b 6f

@ Write opcode and reset p_mem_ram ---------------------------------------------
@ r0    p_mem_ram address
1:  bl rev_comma                        @ Write opcode
    ldr r0, =p_mem_ram
    str tos, [r0]
    DROP                                @ ( p_mem_ram -- )

@ Return -----------------------------------------------------------------------
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

@ Destination for branch -------------------------------------------------------
    bl here                             @ ( -- dest )

@ Return -----------------------------------------------------------------------
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

@ Range check for beq ----------------------------------------------------------
@ r1    pc-relative address (dest - (orig + 4))
    cmp r1, #-1048576                   @ pc-relative address - -1048576
    bge 1f
        DROP                            @ ( orig -- )
        TRACE_WRITE "'shi' conditional branch offset too far negative >>>beq,<<<"
        b 6f                            @ Goto return

1:  ldr r2, =1048574
    cmp r1, r2                          @ pc-relative address - 1048574
    ble 1f                              @ Goto temporarily set p_mem_ram to orig if necessary
        DROP                            @ ( orig -- )
        TRACE_WRITE "'shi' conditional branch offset too far positive >>>beq,<<<"
        b 6f                            @ Goto return

@ Temporarily set p_mem_ram to orig if necessary -------------------------------
@ tos   orig
@ r0    dest
@ r2    p_mem_ram address
@ r3    p_mem_ram
@ r4    flag to indicate whether p_mem_ram is overwritten or not
1:  movs r4, #0                         @ Reset flag
    ldr r2, =p_mem_ram
    ldr r3, [r2]
    cmp tos, r3
    bhs 1f
        movs r4, #1                     @ Set flag
        str tos, [r2]                   @ Temporarily store orig as p_mem_ram
        movs tos, r3
        PUSH_TOS                        @ ( -- p_mem_ram)

@ beq --------------------------------------------------------------------------
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

@ Write opcode, do not reset p_mem_ram -----------------------------------------
@ r4    flag to indicate whether p_mem_ram is overwritten or not
    cmp r4, #0
    bne 1f
        bl rev_comma                    @ Write opcode
        b 6f

@ Write opcode and reset p_mem_ram ---------------------------------------------
@ r0    p_mem_ram address
1:  bl rev_comma                        @ Write opcode
    ldr r0, =p_mem_ram
    str tos, [r0]
    DROP                                @ ( p_mem_ram -- )

@ Return -----------------------------------------------------------------------
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
@ blt,
@ ( orig dest -- )
@ Compile a conditional less-than jump from orig to dest. For future-proofness
@ the 32bit encoding t3 is used as instruction.
 ******************************************************************************/
WORD FLAG_SKIP, "blt,", blt_comma
    push {lr}

@ tos   orig
@ r0    dest
@ r1    pc-relative address (dest - (orig + 4))
    POP_REGS r0                         @ ( dest -- )
    subs r1, r0, tos                    @ dest - orig
    subs r1, #4                         @ pc is 4 bytes ahead in thumb/thumb2!

@ Range check for blt ----------------------------------------------------------
@ r1    pc-relative address (dest - (orig + 4))
    cmp r1, #-1048576                   @ pc-relative address - -1048576
    bge 1f
        DROP                            @ ( orig -- )
        TRACE_WRITE "'shi' conditional branch offset too far negative >>>blt,<<<"
        b 6f                            @ Goto return

1:  ldr r2, =1048574
    cmp r1, r2                          @ pc-relative address - 1048574
    ble 1f                              @ Goto temporarily set p_mem_ram to orig if necessary
        DROP                            @ ( orig -- )
        TRACE_WRITE "'shi' conditional branch offset too far positive >>>blt,<<<"
        b 6f                            @ Goto return

@ Temporarily set p_mem_ram to orig if necessary -------------------------------
@ tos   orig
@ r0    dest
@ r2    p_mem_ram address
@ r3    p_mem_ram
@ r4    flag to indicate whether p_mem_ram is overwritten or not
1:  movs r4, #0                         @ Reset flag
    ldr r2, =p_mem_ram
    ldr r3, [r2]
    cmp tos, r3
    bhs 1f
        movs r4, #1                     @ Set flag
        str tos, [r2]                   @ Temporarily store orig as p_mem_ram
        movs tos, r3
        PUSH_TOS                        @ ( -- p_mem_ram)

@ blt --------------------------------------------------------------------------
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

@ Write opcode, do not reset p_mem_ram -----------------------------------------
@ r4    flag to indicate whether p_mem_ram is overwritten or not
    cmp r4, #0
    bne 1f
        bl rev_comma                    @ Write opcode
        b 6f

@ Write opcode and reset p_mem_ram ---------------------------------------------
@ r0    p_mem_ram address
1:  bl rev_comma                        @ Write opcode
    ldr r0, =p_mem_ram
    str tos, [r0]
    DROP                                @ ( p_mem_ram -- )

@ Return -----------------------------------------------------------------------
6:  pop {pc}

/***************************************************************************//**
@ bl,
@ ( xt -- )
@ Compiles code for a call to xt in the memory-space. The call could either be:
@ bl (4 bytes) pc-relative up to a range of -16777216 to 16777214 or
@ movw movt blx (10 bytes) absolute 32bit
 ******************************************************************************/
WORD FLAG_SKIP, "bl,", bl_comma
    push {lr}

@ Ram or flash -----------------------------------------------------------------
    bl comma_q                          @ ( -- true | false )
    cmp tos, #0
    beq 1f                              @ Goto bl, ram
        b 2f                            @ Goto bl, flash

@ bl, ram ----------------------------------------------------------------------
@ tos   p_mem_ram
@ r0    pc-relative address
@ r2    xt
1:  DROP                                @ ( false -- )
    bl here                             @ ( -- p_mem_ram )
    SWAP                                @ ( xt p_mem_ram -- p_mem_ram xt )
    POP_REGS r2                         @ ( xt -- )
    subs r0, r2, tos                    @ xt - p_mem_ram
    subs r0, #4                         @ pc is 4 bytes ahead in thumb/thumb2!
    b 1f                                @ Goto range check for bl

@ bl, flash --------------------------------------------------------------------
@ tos   xt
@ r0    pc-relative address
@ r2    xt
2:  DROP                                @ ( true -- )
    ldr r0, =p_mem_ram
    ldmia r0, {r1, r2}
    subs r1, r2                         @ Length of current definition
    ldr r0, =p_mem_flash
    ldr r0, [r0]                        @ Beginning of current definition in flash
    adds r0, r1                         @ Address current definition would have in flash so far
    subs r0, tos, r0                    @ pc-relative address
    subs r0, #4                         @ pc is 4 bytes ahead in thumb/thumb2!
    movs r2, tos                        @ Keep xt in r2 for later use

@ Range check for bl -----------------------------------------------------------
@ r0    pc-relative address
1:  cmp r0, #-16777216                  @ pc-relative address - -16777216
    blt 3f                              @ Goto movw movt blx

    ldr r1, =16777214
    cmp r0, r1                          @ pc-relative address - 16777214
    bgt 3f                              @ Goto movw movt blx

@ bl ---------------------------------------------------------------------------
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

@ movw movt blx ----------------------------------------------------------------
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

/***************************************************************************//**
@ bne,
@ ( orig dest -- )
@ Compile a conditional not-equal jump from orig to dest. For future-proofness
@ the 32bit encoding t3 is used as instruction.
 ******************************************************************************/
WORD FLAG_SKIP, "bne,", bne_comma
    push {lr}

@ tos   orig
@ r0    dest
@ r1    pc-relative address (dest - (orig + 4))
    POP_REGS r0                         @ ( dest -- )
    subs r1, r0, tos                    @ dest - orig
    subs r1, #4                         @ pc is 4 bytes ahead in thumb/thumb2!

@ Range check for bne ----------------------------------------------------------
@ r1    pc-relative address (dest - (orig + 4))
    cmp r1, #-1048576                   @ pc-relative address - -1048576
    bge 1f
        DROP                            @ ( orig -- )
        TRACE_WRITE "'shi' conditional branch offset too far negative >>>bne,<<<"
        b 6f                            @ Goto return

1:  ldr r2, =1048574
    cmp r1, r2                          @ pc-relative address - 1048574
    ble 1f                              @ Goto temporarily set p_mem_ram to orig if necessary
        DROP                            @ ( orig -- )
        TRACE_WRITE "'shi' conditional branch offset too far positive >>>bne,<<<"
        b 6f                            @ Goto return

@ Temporarily set p_mem_ram to orig if necessary -------------------------------
@ tos   orig
@ r0    dest
@ r2    p_mem_ram address
@ r3    p_mem_ram
@ r4    flag to indicate whether p_mem_ram is overwritten or not
1:  movs r4, #0                         @ Reset flag
    ldr r2, =p_mem_ram
    ldr r3, [r2]
    cmp tos, r3
    bhs 1f
        movs r4, #1                     @ Set flag
        str tos, [r2]                   @ Temporarily store orig as p_mem_ram
        movs tos, r3
        PUSH_TOS                        @ ( -- p_mem_ram)

@ bne --------------------------------------------------------------------------
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

@ Write opcode, do not reset p_mem_ram -----------------------------------------
@ r4    flag to indicate whether p_mem_ram is overwritten or not
    cmp r4, #0
    bne 1f
        bl rev_comma                    @ Write opcode
        b 6f

@ Write opcode and reset p_mem_ram ---------------------------------------------
@ r0    p_mem_ram address
1:  bl rev_comma                        @ Write opcode
    ldr r0, =p_mem_ram
    str tos, [r0]
    DROP                                @ ( p_mem_ram -- )

@ Return -----------------------------------------------------------------------
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
    ldr r0, =p_mem_ram
    ldr r1, [r0]
    strb tos, [r1], #1                  @ Write char to address in p_mem_ram
    str r1, [r0]                        @ Update address in p_mem_ram
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

@ Create -----------------------------------------------------------------------
    bl create

@ Write literal with the C variables address -----------------------------------
    bl literal

@ bx lr
    PUSH_INT16 #0x4770
    bl h_comma

@ End --------------------------------------------------------------------------
    PUSH_INT8 #FLAG_INTERPRET_COMPILE   @ ( -- flags )
    bl end_colon_semicolon

@ Return -----------------------------------------------------------------------
    pop {pc}

/*
WORD FLAG_SKIP, "c@", c_fetch
    bx lr
*/

/*
WORD FLAG_SKIP, "cell+", cell_plus
    bx lr
*/

/*
WORD FLAG_SKIP, "cells", cells
    bx lr
*/

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

@ Create -----------------------------------------------------------------------
    bl create

@ Write literal ----------------------------------------------------------------
    bl literal

@ bx lr
    PUSH_INT16 #0x4770
    bl h_comma

@ End --------------------------------------------------------------------------
    PUSH_INT8 #FLAG_INTERPRET_COMPILE   @ ( -- flags )
    bl end_colon_semicolon

@ Return -----------------------------------------------------------------------
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
WORD FLAG_INTERPRET, "create"
    push {lr}

@ Parse ------------------------------------------------------------------------
    bl source                           @ ( -- c-addr u )
    bl parse                            @ ( -- token-addr token-u )
    cmp tos, #0                         @ token-u - 0
    bne 1f                              @ Goto find
        TWO_DROP                        @ ( token-addr false -- )
        TRACE_WRITE "'shi' attempt to use zero-length string as a name >>>create<<<"
        b 6f                            @ Goto return

@ Find -------------------------------------------------------------------------
1:  TWO_DUP                             @ ( -- token-addr token-u token-addr token-u )
    bl find                             @ ( -- token-addr 0 | xt flags )
    // TODO maybe write a "create" which doesn't check for redefinition?
    cmp tos, #0                         @ flags - 0
    beq 1f                              @ Goto create
        TWO_DROP                        @ ( xt flags -- )
        TWO_DROP                        @ ( token-addr token-u -- )
        TRACE_WRITE "'shi' redefined word >>>create<<<"
        // TODO What should we do in case a word gets redefined?
        // Maybe src should be dropped, so that when we leave create we also
        // leave evaluate?
        b 6f                            @ Goto return

@ Create -----------------------------------------------------------------------
@ Mark beginning of new definition
@ r0    p_mem_ram address
@ r1    p_mem_ram
1:  ldr r0, =p_mem_ram
    ldr r1, [r0]
    str r1, [r0, #4]

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
    bls 1f                              @ Goto align p_mem_ram
        PUSH_TOS
        ldrb tos, [r0], #1              @ character
        push {r0, r1}
        bl c_comma                      @ Write character
        pop {r0, r1}
        b 1b

@ Align p_mem_ram
@ Name could have been any length and screw with alignment
1:  bl align2

@ Return -----------------------------------------------------------------------
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

@ Do consumes n1 and n2 and pushes it onto the return stack --------------------
@ movs r0, tos
@ ldmia dsp!, {r1, tos}
    ldr r0, =0xCF420030
    PUSH_REGS r0                        @ ( -- opcode )
    bl comma

@ push {r0, r1}
    PUSH_INT16 #0xB403                  @ ( -- opcode )
    bl h_comma

@ Do-sys -----------------------------------------------------------------------
    bl here                             @ ( -- do-sys )

@ Return -----------------------------------------------------------------------
    pop {pc}                            @ return

/*
WORD FLAG_SKIP, "does>", does
    bx lr
*/

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

@ Reserve space for orig2 ------------------------------------------------------
    bl here                             @ ( -- orig2)
    PUSH_INT8 #4
    bl allot

@ Resolve branch from pointer from if or else ----------------------------------
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

@ Return -----------------------------------------------------------------------
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

@ Ram or flash -----------------------------------------------------------------
    bl comma_q                          @ ( -- true | false )
    cmp tos, #0
    beq 1f                              @ Goto end:; ram
        b 2f                            @ Goto end:; flash

@ end:; ram --------------------------------------------------------------------
@ tos   flags
@ r0    p_mem_ram address
@ r1    p_mem_ram
@ r2    p_mem_ram+4
@ r3    l_mem address
@ r4    l_mem
1:  DROP                                @ ( false -- )
    ldr r0, =p_mem_ram
    ldmia r0, {r1, r2}
    ldr r3, =l_mem
    ldr r4, [r3]
    str r2, [r3]                        @ Update last link
    str r4, [r2], #4                    @ Write link
    strb tos, [r2]                      @ Write flags
    b 6f                                @ Goto return

@ end:; flash ------------------------------------------------------------------
@ Align the length of the definition to 8-bytes
@ r0    p_mem_ram address
@ r1    p_mem_ram
@ r2    p_mem_ram+4
@ r3    length | aligned length
@ r4    scratch
2:  DROP                                @ ( true -- )
    ldr r0, =p_mem_ram
    ldmia r0, {r1, r2}
    subs r3, r1, r2                     @ p_mem_ram - p_mem_ram+4
    .if !(FLASH_WRITE_SIZE - 1)
    nop
    .elseif !(FLASH_WRITE_SIZE - 2)
    P2ALIGN1 align=r3, scratch=r4
    .elseif !(FLASH_WRITE_SIZE - 4)
    P2ALIGN2 align=r3, scratch=r4
    .elseif !(FLASH_WRITE_SIZE - 8)
    P2ALIGN3 align=r3, scratch=r4
    .elseif !(FLASH_WRITE_SIZE - 16)
    nop // TODO P2ALIGN4
    .endif
    adds r4, r2, r3
    str r4, [r0]
    cmp r3, #8
    blo 6f

@ Write link and flags
@ r0    p_mem_flash address
@ r1    p_mem_flash
@ r2    p_mem_ram+4
@ r3    aligned length
@ r4    next link
    ldr r0, =p_mem_flash
    ldr r1, [r0]
    adds r4, r1, r3
    str r4, [r2]                        @ Write next link
    strb tos, [r2, #4]                  @ Write flags

@ Unlocking the flash memory
@ r0    FLASH_KEYR register address
@ r3    FLASH_KEY1 | FLASH_KEY2
    ldr r0, =FLASH_KEYR
    ldr r3, =FLASH_KEY1
    str r3, [r0]
    ldr r3, =FLASH_KEY2
    str r3, [r0]

@ Check that no flash main memory operation is ongoing by checking the BSY bit
@ in FLASH_SR
@ r0    FLASH_SR register address
@ r3    FLASH_SR register
@ r5    scratch
    ldr r0, =FLASH_SR
1:  ldr r3, [r0]
    ands r5, r3, #FLASH_SR_BSY
    bne 1b

@ Check and clear all error programming flags due to a previous programming
@ If not, PGSERR is set
@ r0    FLASH_SR register address
@ r3    FLASH_SR register
@ r5    scratch
1:  ands r5, r3, #FLASH_SR_PGS_ERR
    beq 1f
        ands r3, #!FLASH_SR_OPTV_ERR
        ands r3, #!FLASH_SR_RD_ERR
        ands r3, #!FLASH_SR_FAST_ERR
        ands r3, #!FLASH_SR_MISS_ERR
        ands r3, #!FLASH_SR_PGS_ERR
        ands r3, #!FLASH_SR_SIZ_ERR
        ands r3, #!FLASH_SR_PGA_ERR
        ands r3, #!FLASH_SR_WRP_ERR
        ands r3, #!FLASH_SR_PROG_ERR
        ands r3, #!FLASH_SR_OP_ERR
        str r3, [r0]
        ldr r3, [r0]
        b 1b

@ Set the PG bit in FLASH_CR
@ r0    FLASH_CR register address
@ r3    FLASH_CR
1:  ldr r0, =FLASH_CR
    ldr r3, [r0]
    orrs r3, #FLASH_CR_PG
    str r3, [r0]

@ Copy definition from ram to flash
@ r0    FLASH_SR register address | p_mem_flash address
@ r1    p_mem_flash
@ r2    p_mem_ram+4
@ r3    first word | scratch
@ r4    next link
@ r5    second word
    ldr r0, =FLASH_SR
1:  ldr r3, [r2], #4                    @ DO NOT use ldm, there is no guarantee that r2 is aligned!
    ldr r5, [r2], #4
    str r3, [r1], #4                    @ Write first word
    str r5, [r1], #4                    @ Write second word

2:  ldr r3, [r0]                        @ Wait until the BSY bit is cleared
    ands r5, r3, #FLASH_SR_BSY
    bne 2b

    cmp r1, r4                          @ Check if we're done
    blo 1b

    ldr r0, =p_mem_flash                @ Update p_mem_flash
    str r1, [r0]

@ Clear the PG bit in the FLASH_SR register if there are no more programming
@ requests
@ r0    FLASH_CR register address
@ r3    scratch
    ldr r0, =FLASH_CR
    ldr r3, [r0]
    ands r3, #!FLASH_CR_PG
    str r3, [r0]

@ Locking the flash memory
@ r0    FLASH_CR register address
@ r3    scratch
    ldr r0, =FLASH_CR
    ldr r3, [r0]
    orrs r3, #FLASH_CR_LOCK
    str r3, [r0]

@ Clear definition from ram
@ r0    p_mem_ram address
@ r1    p_mem_ram
@ r2    p_mem_ram+4
@ r3    erased word
    ldr r0, =p_mem_ram
    ldmia r0, {r1, r2}
    movs r3, #ERASED_WORD
1:  str r3, [r1], #-4
    cmp r1, r2
    bhs 1b
    str r2, [r0]

@ Return -----------------------------------------------------------------------
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

@ Store source -----------------------------------------------------------------
@ tos   u
@ r0    c-addr
@ r1    src address
    ldr r0, [dsp]
    ldr r1, =src
    stmia r1, {r0, tos}
    TWO_DROP                            @ ( c-addr u -- )

@ Set >IN 0 --------------------------------------------------------------------
    SET_IN #0                           @ Set >in zero

@ Interpret --------------------------------------------------------------------
    bl interpret                        @ Start interpreter

@ Return -----------------------------------------------------------------------
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

@ return -----------------------------------------------------------------------
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
    ldr r1, =l_mem                      @ Begin search at latest link
    POP_REGS r0                         @ ( token-u -- )

@ Search -----------------------------------------------------------------------
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

@ Found nothing ----------------------------------------------------------------
4:  PUSH_INT8 #0                        @ ( -- 0 )
    b 6f                                @ Goto return

@ Found something
@ Only keep tos, r2 and r4
5:  P2ALIGN1 r4                         @ xt is at next 2-byte aligned address
    DROP                                @ ( token-addr -- )
    PUSH_REGS top=r2, from=r4           @ ( -- xt flags )

@ Return -----------------------------------------------------------------------
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
    ldr r0, =p_mem_ram
    ldr r1, [r0]
    strh tos, [r1], #2                  @ Write h to address in p_mem_ram
    str r1, [r0]                        @ Update address in p_mem_ram
    DROP                                @ ( h -- )
    bx lr

/***************************************************************************//**
@ here
@ ( -- addr )
@ addr is the memory-space pointer.
 ******************************************************************************/
WORD FLAG_SKIP, "here"
    PUSH_TOS
    ldr tos, =p_mem_ram
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

@ Return -----------------------------------------------------------------------
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

@ If needs to consume its own flag ---------------------------------------------
@ movs r0, tos
@ ldmia dsp!, {tos}
    ldr r0, =0xCF400030
    PUSH_REGS r0                        @ ( -- opcode )
    bl comma

@ cmp r0, #0
    PUSH_INT16 #0x2800                  @ ( -- opcode )
    bl h_comma

@ Orig and pointer to branch function ------------------------------------------
    bl here                             @ ( -- orig )
    ldr r0, =beq_comma
    PUSH_REGS r0                        @ ( -- fp-to-branch)

@ Reserve space for orig -------------------------------------------------------
    PUSH_INT8 #4                        @ Reserve space for branch instruction
    bl allot

@ Return -----------------------------------------------------------------------
    pop {pc}
.ltorg

/*
WORD FLAG_SKIP, "immediate"
    bx lr
*/

/***************************************************************************//**
@ inline,
@ ( xt -- )
@ Inlines code from xt in the memory-space.
 ******************************************************************************/
WORD FLAG_SKIP, "inline,", inline_comma
    push {lr}

@ Copy opcodes from xt ---------------------------------------------------------
@ r0    xt
@ r1    opcode bx lr
@ r2    hword
    POP_REGS r0
    movw r1, #0x4770
1:  ldrh r2, [r0], #2
    cmp r2, r1                          @ End if opcode equals bx lr
    beq 6f
    cmp r2, #0xBD00                     @ or pop {pc}
    beq 6f
    PUSH_REGS r2
    push {r0, r1}
    bl h_comma
    pop {r0, r1}
    b 1b

@ Return -----------------------------------------------------------------------
6:  pop {pc}

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

@ Return -----------------------------------------------------------------------
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

@ Unloop -----------------------------------------------------------------------
    bl unloop

@ Orig -------------------------------------------------------------------------
    bl here                             @ ( -- orig )

@ Reserve space for orig -------------------------------------------------------
    PUSH_INT8 #4                        @ Reserve space for branch instruction
    bl allot

@ Reverse push orig onto the stack ---------------------------------------------
@ r0    p_structure address
@ r1    p_structure
    ldr r0, =p_structure
    ldr r1, [r0]
    stmia r1!, {tos}
    str r1, [r0]
    DROP                                @ ( orig -- )

@ Return -----------------------------------------------------------------------
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
@ itt lt
    ldr r0, =0xBFBC4288
    PUSH_REGS r0                        @ ( -- opcode )
    bl comma

@ pushlt {r0, r1}
    PUSH_INT16 #0xB403                  @ ( -- opcode )
    bl h_comma

@ Call branch function ---------------------------------------------------------
    bl here                             @ ( -- orig )
    SWAP
    bl blt_comma

@ Leave ------------------------------------------------------------------------
@ r0    p_structure address
@ r1    p_structure
@ r2    stack address
    ldr r0, =p_structure
    ldr r1, [r0]
    ldr r2, =_s_shi_dstack
    cmp r1, r2
    beq 6f

@ Check if p_structure and dsp clash
@ r1    p_structure
    cmp r1, dsp
    blo 1f
        TRACE_WRITE "'shi' stack overflow >>>leave<<<"
        b 6f

@ Take care of leave(s)
@ r0    p_structure address
@ r1    p_structure
@ r2    orig
@ r3    scratch
1:  ldr r2, [r1, #-4]
    PUSH_REGS r2                        @ ( -- orig )
    movs r3, #0
    str r3, [r1, #-4]!
    str r1, [r0]
    bl here                             @ ( -- dest )
    bl b_comma

@ Return -----------------------------------------------------------------------
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

@ Resolve branch to begin ------------------------------------------------------
    bl here                             @ ( -- orig )
    SWAP
    bl b_comma

@ Resolve branch from while ----------------------------------------------------
    bl here                             @ ( -- dest )
    bl beq_comma

@ Return -----------------------------------------------------------------------
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
    ldr r0, =p_mem_ram
    ldr r1, [r0]
    movs r2, tos
    lsrs tos, #16
    strh tos, [r1], #2                  @ Write x to address in p_mem_ram
    strh r2, [r1], #2                   @ Write x to address in p_mem_ram
    str r1, [r0]                        @ Update address in p_mem_ram
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

@ Destination for branch -------------------------------------------------------
    bl here                             @ ( -- dest )

@ Resolve branch from pointer from if or else ----------------------------------
@ r0    fp-to-branch
@ r1    dest
    POP_REGS top=r1, to=r0              @ ( orig fp-to-branch dest -- )
    PUSH_REGS r1
    blx r0

@ Return -----------------------------------------------------------------------
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

@ Return -----------------------------------------------------------------------
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

@ Until needs to consume its own flag ------------------------------------------
@ movs r0, tos
@ ldmia dsp!, {tos}
    ldr r0, =0xCF400030
    PUSH_REGS r0                        @ ( -- opcode )
    bl comma

@ cmp r0, #0
    PUSH_INT16 #0x2800                  @ ( -- opcode )
    bl h_comma

@ Resolve branch to begin ------------------------------------------------------
    bl here                             @ ( -- orig )
    SWAP
    bl beq_comma

@ Return -----------------------------------------------------------------------
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

@ Create -----------------------------------------------------------------------
    bl create

@ Write literal with the reserved cells address --------------------------------
    ldr r0, =p_variable
    ldr r1, [r0]
    movs r2, #0                         @ Zero initialize cell
    str r2, [r1]
    PUSH_REGS r1
    subs r1, #4
    str r1, [r0]
    bl literal

@ bx lr
    PUSH_INT16 #0x4770
    bl h_comma

@ End --------------------------------------------------------------------------
    PUSH_INT8 #FLAG_INTERPRET_COMPILE & RESERVE_1CELL   @ ( -- flags )
    bl end_colon_semicolon

@ Return -----------------------------------------------------------------------
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

@ While needs to consume its own flag ------------------------------------------
@ movs r0, tos
@ ldmia dsp!, {tos}
    ldr r0, =0xCF400030
    PUSH_REGS r0                        @ ( -- opcode )
    bl comma

@ cmp r0, #0
    PUSH_INT16 #0x2800                  @ ( -- opcode )
    bl h_comma

@ Reserve space for orig -------------------------------------------------------
    bl here                             @ ( -- orig )
    SWAP
    PUSH_INT8 #4                        @ Reserve space for branch instruction
    bl allot

@ Return -----------------------------------------------------------------------
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

@ ' ----------------------------------------------------------------------------
    bl tick                             @ ( -- xt )
    bl literal                          @ ( xt -- )

@ Return -----------------------------------------------------------------------
6:  pop {pc}

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

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ Core extension words @@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/*
WORD FLAG_SKIP, ".(", dot_comment
    bx lr
*/

/*
WORD FLAG_SKIP, ".r", dot_r
    bx lr
*/

/***************************************************************************//**
@ 0<>
@ ( x -- flag )
@ flag is true if and only if x is not equal to zero.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "0<>", zero_ne
    cmp tos, #0
    ite ne
    movne tos, #-1
    moveq tos, #0
    bx lr

/***************************************************************************//**
@ 0>
@ ( n -- flag )
@ flag is true if and only if n is greater than zero.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "0>", zero_more
    cmp tos, #0
    ite gt
    movgt tos, #-1
    movle tos, #0
    bx lr

/***************************************************************************//**
@ 2>r
@ (    x1 x2 --       )
@ ( R:       -- x1 x2 )
@ Transfer cell pair x1 x2 to the return stack. Semantically equivalent to swap
@ >r >r.
 ******************************************************************************/
WORD FLAG_COMPILE & FLAG_INLINE, "2>r", two_to_r
    TWO_TO_R
    bx lr

/***************************************************************************//**
@ 2>r
@ (         -- x1 x2 )
@ (R: x1 x2 --       )
@ Transfer cell pair x1 x2 from the return stack. Semantically equivalent to r>
@ r> swap.
 ******************************************************************************/
WORD FLAG_COMPILE & FLAG_INLINE, "2r>", two_r_from
    TWO_R_FROM
    bx lr

/***************************************************************************//**
@ 2r@
@ (          -- x1 x2 )
@ ( R: x1 x2 -- x1 x2 )
@ Copy cell pair x1 x2 from the return stack. Semantically equivalent to r> r>
@ 2dup >r >r swap.
 ******************************************************************************/
WORD FLAG_COMPILE & FLAG_INLINE, "2r@", two_r_fetch
    ldmia sp, {r0, r1}
    PUSH_REGS top=r0, from=r1
    bx lr

/*
WORD FLAG_SKIP, ":noname", colon_noname
    bx lr
*/

/***************************************************************************//**
@ <>
@ ( x1 x2 -- flag )
@ flag is true if and only if x1 is not bit-for-bit the same as x2.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FOLDS_2, "<>", ne
    ldmia dsp!, {r0}
    cmp r0, tos
    ite ne
    movne tos, #-1
    moveq tos, #0
    bx lr

/*
WORD FLAG_SKIP, "?do", q_do
    bx lr
*/

/*
WORD FLAG_SKIP, "action-of", action_of
    bx lr
*/

/***************************************************************************//**
@ again
@ ( dest -- )
@ Append the run-time semantics given below to the current definition, resolving
@ the backward reference dest.
@
@ ( -- )
@ Continue execution at the location specified by dest. If no other control flow
@ words are used, any program code after again will not be executed.
 ******************************************************************************/
WORD FLAG_COMPILE_IMMEDIATE, "again"
    push {lr}

@ Resolve branch to begin ------------------------------------------------------
    bl here                             @ ( -- orig )
    SWAP
    bl b_comma

@ Return -----------------------------------------------------------------------
    pop {pc}

/*
WORD FLAG_SKIP, "buffer:", buffer_colon
    bx lr
*/

/*
WORD FLAG_SKIP, "c\"", c_q
    bx lr
*/

/***************************************************************************//**
@ case
@ ( -- case-sys )
@ Mark the start of the case...of...endof...endcase structure. Append the
@ run-time semantics given below to the current definition.
@
@ ( -- )
@ Continue execution.
 ******************************************************************************/
WORD FLAG_COMPILE_IMMEDIATE, "case"
    bx lr

/*
WORD FLAG_SKIP, "defer"
    bx lr
*/

/*
WORD FLAG_SKIP, "defer!", defer_store
    bx lr
*/

/*
WORD FLAG_SKIP, "defer@", defer_fetch
    bx lr
*/

/***************************************************************************//**
@ endcase
@ ( case-sys -- )
@ Mark the end of the case...of...endof...endcase structure. Use case-sys to
@ resolve the entire structure. Append the run-time semantics given below to the
@ current definition.
@
@ ( x -- )
@ Discard the case selector x and continue execution
 ******************************************************************************/
WORD FLAG_COMPILE_IMMEDIATE, "endcase"
    push {lr}

@ If the case selector never matched, discard it now ---------------------------
@ before we resolve the branch(es) from endof(s)
@ ldmia dsp!, {tos}
    PUSH_INT16 #0xCF40
    bl h_comma

@ Resolve branch(es) from endof(s) ---------------------------------------------
@ r0    p_structure address
@ r1    p_structure
@ r2    stack address
    ldr r0, =p_structure
    ldr r1, [r0]
    ldr r2, =_s_shi_dstack
    cmp r1, r2
    beq 6f

@ Check if p_structure and dsp clash
@ r1    p_structure
    cmp r1, dsp
    blo 1f
        TRACE_WRITE "'shi' stack overflow >>>endcase<<<"
        b 6f

@ Take care of endof(s)
@ r0    p_structure address
@ r1    p_structure
@ r2    scratch
1:  ldr r2, [r1, #-4]
    PUSH_REGS r2                        @ ( -- orig )
    movs r2, #0
    str r2, [r1, #-4]!
    push {r0, r1}
    bl here                             @ ( -- dest )
    bl b_comma
    pop {r0, r1}

    ldr r2, =_s_shi_dstack
    cmp r1, r2
    bhi 1b
    str r2, [r0]

@ Return -----------------------------------------------------------------------
6:  pop {pc}

/***************************************************************************//**
@ endof
@ ( case-sys1 of-sys -- case-sys2 )
@ Mark the end of the of...endof part of the case structure. The next location
@ for a transfer of control resolves the reference given by of-sys. Append the
@ run-time semantics given below to the current definition. Replace case-sys1
@ with case-sys2 on the control-flow stack, to be resolved by endcase.
@
@ ( -- )
@ Continue execution at the location specified by the consumer of case-sys2.
 ******************************************************************************/
WORD FLAG_COMPILE_IMMEDIATE, "endof"
    push {lr}

@ Reverse push orig onto the stack ---------------------------------------------
@ r0    p_structure address
@ r1    p_structure
    bl here                             @ ( -- orig )
    ldr r0, =p_structure
    ldr r1, [r0]
    stmia r1!, {tos}
    str r1, [r0]
    DROP                                @ ( orig -- )

@ Reserve space for orig -------------------------------------------------------
    PUSH_INT8 #4
    bl allot

@ Resolve branch from of -------------------------------------------------------
    bl here                             @ ( -- dest )
    bl bne_comma

@ Return -----------------------------------------------------------------------
    pop {pc}

/*
WORD FLAG_SKIP, "erase"
    bx lr
*/

/***************************************************************************//**
@ false
@ ( -- false )
@ Return a false flag.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE, "false"
    PUSH_TOS
    movs tos, #0
    bx lr

/***************************************************************************//**
@ hex
@ ( -- )
@ Set contents of radix to sixteen.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE, "hex"
    ldr r0, =radix
    movs r1, #16
    str r1, [r0]
    bx lr

/*
WORD FLAG_SKIP, "holds"
    bx lr
*/

/*
WORD FLAG_SKIP, "is"
    bx lr
*/

/*
WORD FLAG_SKIP, "marker"
    bx lr
*/

/***************************************************************************//**
@ nip
@ ( x1 x2 -- x2 )
@ Drop the first item below the top of stack.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "nip"
    NIP
    bx lr

/***************************************************************************//**
@ of
@ ( -- of-sys )
@ Put of-sys onto the stack. Append the run-time semantics given below to the
@ current definition. The semantics are incomplete until resolved by a consumer
@ of of-sys such as endof.
@
@ ( x1 x2 -- | x1 )
@ If the two values on the stack are not equal, discard the top value and
@ continue execution at the location specified by the consumer of of-sys,
@ e.g., following the next endof. Otherwise, discard both values and continue
@ execution in line.
 ******************************************************************************/
WORD FLAG_COMPILE_IMMEDIATE, "of"
    push {lr}

@ movs r0, tos
@ ldmia dsp!, {tos}
    ldr r0, =0xCF400030
    PUSH_REGS r0
    bl comma

@ cmp r0, tos
    PUSH_INT16 #0x42B0
    bl h_comma

@ of-sys -----------------------------------------------------------------------
    bl here                             @ ( -- of-sys )
    PUSH_INT8 #4
    bl allot

@ ldmia dsp!, {tos}
    PUSH_INT16 #0xCF40
    bl h_comma

@ Return -----------------------------------------------------------------------
    pop {pc}

/*
WORD FLAG_SKIP, "pad"
    bx lr
*/

/***************************************************************************//**
@ parse
@ ( c-addr u -- token-addr token-u )
@ c-addr is the address (within the input buffer) and u is the length of the
@ parsed string. If the parse area was empty, the resulting string has a zero
@ length.
@
@ I think the standard version is supposed to work with the input string
@ directly ( char "ccc<char>" -- c-addr u ). This version simply takes the
@ whole cstring from stack, searches for the next token with respect to >in and
@ then simply throws the found token, also as cstring, back onto the stack.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE, "parse"

@ tos   c-addr + >in            (= start address)
@ r0    c-addr + >in advanced   (= current address)
@ r1    c-addr + u              (= end address)
@ r2    >in
@ r3    character
    ldr r2, =in                         @ >in
    ldr r2, [r2]
    POP_REGS r1                         @ ( u -- )

    adds r1, tos, r1                    @ c-addr + u
    adds tos, tos, r2                   @ c-addr + >in
    movs r0, tos                        @ Keep start address

@ Skip leading spaces ----------------------------------------------------------
1:  cmp r1, r0                          @ (c-addr + u) - (c-addr + >in)
    bls 3f                              @ Goto out of characters
        ldrb r3, [r0], #1               @ Get character and increment c-addr + >in
        cmp r3, #' '                    @ character - ' '
        bne 2f
            adds tos, #1                @ Increment start address to make up for leading spaces
            adds r2, #1                 @ Also increment >in to make up for leading spaces
            b 1b                        @ Goto skip leading spaces

@ Compare against space delimiter
2:  cmp r1, r0                          @ (c-addr + u) - (c-addr + >in)
    bls 3f                              @ Goto out of characters
        ldrb r3, [r0], #1               @ Get character and increment c-addr + >in
        cmp r3, #' '                    @ character - ' '
        bne 2b                          @ Goto compare against space delimiter

@ Space delimiter found
    subs r0, tos                        @ Length of found token + space
    adds r2, r0                         @ Add found token to >in
    ldr r1, =in                         @ Advance >in and store it
    str r2, [r1]
    subs r0, #1                         @ Subtract space (which isn't part of the token)
    PUSH_REGS r0                        @ ( -- u )
    b 6f                                @ Goto return

@ Out of characters (equals \r or \n)
3:  subs r0, tos                        @ Length of found token
    adds r2, r0                         @ Add found token to >in
    ldr r1, =in                         @ Advance >in and store it
    str r2, [r1]
    PUSH_REGS r0

@ Return -----------------------------------------------------------------------
6:  bx lr

/*
WORD FLAG_SKIP, "parse-name", parse_name
    bx lr
*/

/***************************************************************************//**
@ pick
@ ( xu...x1 x0 u -- xu...x1 x0 xu )
@ Remove u. Copy the xu to the top of the stack. An ambiguous condition exists
@ if there are less than u+2 items on the stack before pick is executed.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE, "pick"
    PICK
    bx lr

/*
WORD FLAG_SKIP, "refill"
    bx lr
*/

/*
WORD FLAG_SKIP, "restore-input", restore_input
    bx lr
*/

/*
WORD FLAG_SKIP, "roll"
    bx lr
*/

/*
WORD FLAG_SKIP, "s\"", s_eq
    bx lr
*/

/*
WORD FLAG_SKIP, "save-input", save_input
    bx lr
*/

/*
WORD FLAG_SKIP, "source-id", source_id
    bx lr
*/

/*
WORD FLAG_SKIP, "to"
    bx lr
*/

/***************************************************************************//**
@ true
@ ( -- true )
@ Return a true flag, a single-cell value with all bits set.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE, "true"
    PUSH_TOS
    movs tos, #-1
    bx lr

/***************************************************************************//**
@ tuck
@ ( x1 x2 -- x2 x1 x2 )
@ Copy the first (top) stack item below the second stack item.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "tuck"
    TUCK
    bx lr

/*
WORD FLAG_SKIP, "u.r", u_dot_r
    bx lr
*/


/***************************************************************************//**
@ u>
@ ( u1 u2 -- flag )
@ flag is true if and only if u1 is greater than u2.
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE & FOLDS_2, "u>", u_more
    ldmia dsp!, {r0}
    cmp r0, tos
    ite hi
    movhi tos, #-1
    movls tos, #0
    bx lr

/***************************************************************************//**
@ unused
@ ( -- u )
@ u is the amount of space remaining in the region addressed by here, in address
@ units.
 ******************************************************************************/
WORD FLAG_SKIP, "unused"
    push {lr}

@ Ram or flash -----------------------------------------------------------------
    bl comma_q                          @ ( -- true | false )
    cmp tos, #0
    beq 1f                              @ Goto end:; ram
        b 2f                            @ Goto end:; flash

@ unused ram -------------------------------------------------------------------
@ tos   p_variable
@ r0    p_mem_ram
1:  ldr tos, =p_variable
    ldr tos, [tos]
    ldr r0, =p_mem_ram
    ldr r0, [r0]
    subs tos, r0

@ unused flash -----------------------------------------------------------------
@ tos   FLASH_END
@ r0    p_mem_flash
2:  ldr tos, =FLASH_END
    ldr r0, =p_mem_flash
    ldr r0, [r0]
    subs tos, r0

@ Return -----------------------------------------------------------------------
    pop {pc}

/*
WORD FLAG_SKIP, "value"
    bx lr
*/

/*
WORD FLAG_SKIP, "within"
    bx lr
*/

/*
WORD FLAG_SKIP, "[compile]", bracket_compile
    bx lr
*/

/*
WORD FLAG_SKIP, "\\", bs
    bx lr
*/

@@@@@@@@@@@@@@@@@@@@@@@@@ System implementation words @@@@@@@@@@@@@@@@@@@@@@@@@@
/***************************************************************************//**
@ interpret
@ ( -- )
 ******************************************************************************/
WORD FLAG_INTERPRET_COMPILE, "interpret"
    push {lr}

// TODO maybe check for stackoverflow or underflow here?

@ Parse ------------------------------------------------------------------------
interpret_parse:
    bl source                           @ ( -- c-addr u )
    bl parse                            @ Parse string
    cmp tos, #0                         @ token-u - 0
    bne interpret_find                  @ Goto find
        TWO_DROP                        @ ( token-addr 0 -- )
        TRACE_WRITE "'shi' attempt to parse zero-length string >>>interpret<<<"
        b interpret_return              @ Goto return

@ Find -------------------------------------------------------------------------
interpret_find:
    SWAP                                @ ( token-addr token-u -- token-u token-addr )
    OVER                                @ ( token-u token-addr -- token-u token-addr token-u )
    bl find                             @ Find token
    cmp tos, #0                         @ flags - 0
    bne interpret_state                 @ Goto state check

@ Number -----------------------------------------------------------------------
interpret_number:
    DROP                                @ ( false -- )
    SWAP                                @ ( token-u token-addr -- token-addr token-u )
    bl number                           @ ( -- n true | false ) try numbers if we didn't find anything
    POP_REGS r0                         @ ( flag -- )
    cmp r0, #0                          @ flag - 0
    bne interpret_set_lfp               @ Goto set literal-folding pointer
    TRACE_WRITE "'shi' undefined word >>>interpret<<<"
        b interpret_return              @ Goto return

@ Set literal-folding pointer --------------------------------------------------
interpret_set_lfp:
    cmp lfp, #0                         @ Save stackpointer in case literal-folding pointer is still zero
    it eq
    movseq lfp, dsp
    b interpret_done                    @ Goto done

@ State check ------------------------------------------------------------------
@ Decide whether to interpret or to compile
@ r0    status
interpret_state:
    ROT                                 @ ( token-u xt flags -- xt flags token-u )
    DROP                                @ ( token-u -- )
    bl state                            @ ( -- state )
    ldr r0, [tos]
    DROP                                @ ( state -- )
    cmp r0, #0                          @ status - 0
    bne interpret_compile               @ Goto compile

@ Interpret --------------------------------------------------------------------
interpret_interpret:
    ands tos, #~FLAG_INTERPRET
    beq 1f                              @ Goto execute
        TRACE_WRITE "'shi' interpreting a compile-only word >>>interpret<<<"
        b interpret_return              @ Goto return

@ Execute
1:  movs lfp, #0                        @ Clear literal-folding pointer
    DROP                                @ ( flags -- )
    bl execute                          @ Execute xt
        b interpret_done                @ Goto done

@ Compile ----------------------------------------------------------------------
interpret_compile:
    ands r0, tos, #~FLAG_COMPILE
    beq 1f                              @ Goto continue
        TRACE_WRITE "'shi' compiling an interpret-only word >>>interpret<<<"
        b interpret_return              @ Goto return

@ Continue
@ r0    xt
@ r1    flags
1:  POP_REGS top=r1, to=r0              @ ( xt flags -- )
    cmp lfp, #0                         @ literal-folding pointer - 0
    beq 2f                              @ Goto immediate?

@ Literals?
@ r1    flags
@ r2    last literal address
@ r3    #literals
@ r4    folds bits
    subs r2, dsp, #4                    @ Get number of literals on stack
    subs r3, lfp, r2
    lsrs r3, #2
    mvns r4, r1                         @ Invert flags
    ands r4, r4, #BIT_FOLDS             @ Extract folds bits from flags
    beq 1f                              @ Goto literal
        cmp r4, r3                      @ folds bits - number of literals
        bls 3f                          @ Goto execute

@ Literal
@ tos   literal
@ r0    xt
@ r1    flags
@ r2    last literal address
1:  push {r0, r1, lfp}                  @ Save xt, flags and literal-folding pointer
1:  PUSH_TOS
    ldr tos, [lfp, #-4]!
    push {r2}
    bl literal
    pop {r2}
    cmp r2, lfp
    blo 1b
        pop {r0, r1, dsp}               @ Set data-stack pointer past literals
        DROP                            @ ( literal -- )
        movs lfp, #0                    @ Clear literal-folding pointer

@ Immediate?
@ r1    flags
2:  ands r2, r1, #~FLAG_IMMEDIATE
    bne 4f                              @ Goto call, or inline,?

@ Execute (either we got literals and we are foldable, or we are immediate)
@ r0    xt
3:  PUSH_REGS r0                        @ ( -- xt )
    bl execute
        b interpret_done                @ Goto done

@ Call, or inline,?
@ r0    xt
@ r1    flags
4:  PUSH_REGS r0                        @ ( -- xt )
    ands r1, #~FLAG_INLINE
    bne 5f
        bl inline_comma
        b interpret_done                @ Goto done
5:  bl bl_comma

@ Done -------------------------------------------------------------------------
@ tos   >in address
@ r0    c-addr
@ r1    u
@ r2    >in
interpret_done:
    bl source
    POP_REGS top=r1, to=r0
    bl to_in
    ldr r2, [tos]
    DROP                                @ ( >in address -- )
    cmp r1, r2                          @ (c-addr) u - token-u
    bls interpret_return                @ Goto return
        b interpret_parse               @ Goto parse

@ Return -----------------------------------------------------------------------
interpret_return:
    SET_SOURCE #0, #0                   @ Clear source
    pop {pc}
.ltorg
