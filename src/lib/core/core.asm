@ Core word set
@
@ \file   core.asm
@ \author Vincent Hamp
@ \date   27/07/2016

.section .text

@ ------------------------------------------------------------------------------
@ !
@ ( x a-addr -- )
@ Store x at a-addr.
@ ------------------------------------------------------------------------------
.if ENABLE_STORE == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE, "!", store
    ldmia dsp!, {r0, r1}                @ ( x a-addr -- )
    str r0, [tos]
    movs tos, r1
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ '
@ ( source: "<spaces>name" --    )
@ (                        -- xt )
@ Skip leading space delimiters. Parse name delimited by a space. Find name and
@ return xt, the execution token for name. An ambiguous condition exists if name
@ is not found. When interpreting, ' xyz execute is equivalent to xyz.
@ ------------------------------------------------------------------------------
.if ENABLE_TICK == 1
WORD FLAG_INTERPRET, "'", tick
    push {lr}

@ Parse
    bl source                           @ ( -- c-addr u )
    bl parse_name                       @ ( c-addr u -- token-addr token-u )
    cmp tos, #0                         @ token-u - 0
    bne 1f                              @ Goto find
        TWO_DROP                        @ ( token-addr token-u -- )
        PRINT "' zero-length string as a name"
        b 6f                            @ Goto return

@ Find
1:  bl find                             @ ( token-addr token-u -- token-addr false | xt flags )
    cmp tos, #0                         @ flags - 0
    bne 1f                              @ Goto xt
        TWO_DROP                        @ ( token-addr false -- )
        PRINT "' undefined word"
        b 6f                            @ Goto return

@ xt
1:  DROP                                @ ( xt flags -- xt )

@ Return
6:  pop {pc}
.endif

@ ------------------------------------------------------------------------------
@ *
@ ( n1 n2 -- n3 )
@ Multiply n1 by n2 giving the product n3
@ ------------------------------------------------------------------------------
.if ENABLE_TIMES == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "*", times
    ldmia dsp!, {r0}                    @ ( n1 n2 -- n2 )
    muls tos, r0                        @ ( n2 -- n3 )
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ * /
@ ( n1 n2 n3 -- n4 )
@ Multiply n1 by n2 producing the intermediate double-cell result d. Divide d by
@ n3 giving the single-cell quotient n4. An ambiguous condition exists if n3 is
@ zero or if the quotient n4 lies outside the range of a signed number. If d and
@ n3 differ in sign, the implementation-defined result returned will be the same
@ as that returned by either the phrase >r m* r> fm/mod swap drop or the phrase
@ >r m* r> sm/rwm swap drop.
@ ------------------------------------------------------------------------------
.if ENABLE_TIMES_DIV == 1
WORD FLAG_INTERPRET_COMPILE & FOLDS_3, "*/", times_div
@ r0    n1
@ r1    n2
@ r2    n3
@ r3    bottom
@ r12   top
    POP_REGS top=r2, to="r0-r1"         @ ( n1 n2 n3 -- )
    PUSH_TOS
    smull r3, r12, r0, r1;
    cmp r12, #0
    beq 1f                              @ Goto 32bit division
    // TODO 64bit division

@ 32bit division
1:  sdiv tos, r3, r2

@ Return
    bx lr
.endif

.if ENABLE_TIMES_DIV_MOD == 1
WORD FLAG_SKIP, "*/mod", times_div_mod
.endif

@ ------------------------------------------------------------------------------
@ +
@ ( n1 n2 -- n3 )
@ Add n2 to n1, giving the sum n3
@ ------------------------------------------------------------------------------
.if ENABLE_PLUS == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "+", plus
    ldmia dsp!, {r0}                    @ ( n1 n2 -- n2 )
    adds tos, r0                        @ ( n2 -- n3 )
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ +!
@ ( n a-addr -- )
@ Add n to the single-cell number at a-addr.
@ ------------------------------------------------------------------------------
.if ENABLE_PLUS_STORE == 1
WORD FLAG_INTERPRET_COMPILE, "+!", plus_store
    ldmia dsp!, {r0, r1}                @ ( n a-addr -- )
    ldr r2, [tos]
    adds r0, r2
    str r0, [tos]
    movs tos, r1
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ +loop
@ (         dest -- )
@ ( C: leave-sys -- )
@ Append the run-time semantics given below to the current definition. Resolve
@ the destination of all unresolved occurrences of leave between the location
@ given by leave-sys and the next location for a transfer of control, to execute
@ the words following +loop.
@
@ (            n --             )
@ ( R: loop-sys1 -- | loop-sys2 )
@ An ambiguous condition exists if the loop control parameters are unavailable.
@ Add n to the loop index. If the loop index did not cross the boundary between
@ the loop limit minus one and the loop limit, continue execution at the
@ beginning of the loop. Otherwise, discard the current loop control parameters
@ and continue execution immediately following the loop.
@ ------------------------------------------------------------------------------
.if ENABLE_PLUS_LOOP == 1
WORD FLAG_COMPILE_IMMEDIATE, "+loop", plus_loop
    push {lr}

@ pop {r0, r1}
@ subs r2, r1, r0
    PUSH_TOS
    ldr tos, =0x1A0ABC03                @ ( -- opcode )
    bl comma                            @ ( opcode -- )

@ adds r0, r0, tos
@ ldmia dsp!, {tos}
    PUSH_TOS
    ldr tos, =0xCF401980                @ ( -- opcode )
    bl comma                            @ ( opcode -- )

@ subs r3, r1, r0
@ cmp r2, #0
    PUSH_TOS
    ldr tos, =0x2A001A0B                @ ( -- opcode )
    bl comma                            @ ( opcode -- )

@ ite le
@ movle r2, #0
    PUSH_TOS
    ldr tos, =0x2200BFD4                @ ( -- opcode )
    bl comma                            @ ( opcode -- )

@ movgt r2, #1
@ cmp r3, #0
    PUSH_TOS
    ldr tos, =0x2B002201                @ ( -- opcode )
    bl comma                            @ ( opcode -- )

@ ite gt
@ movgt r3, r2
    PUSH_TOS
    ldr tos, =0x4613BFCC                @ ( -- opcode )
    bl comma                            @ ( opcode -- )

@ eorle r3, r2, #1
    PUSH_TOS
    ldr tos, =0x0301F082                @ ( -- opcode )
    bl comma                            @ ( opcode -- )

@ cmp r3, #0
@ it ne
    PUSH_TOS
    ldr tos, =0xBF182B00                @ ( -- opcode )
    bl comma                            @ ( opcode -- )

@ pushne {r0, r1}
    PUSH_INT16 #0xB403                  @ ( -- opcode )
    bl h_comma                          @ ( opcode -- )

@ Resolve dest
    bl here                             @ ( dest -- dest orig )
    SWAP                                @ ( dest orig -- orig dest )
    PUSH_TOS                            @ bne
    ldr tos, =0xF0408000                @ ( orig dest -- orig dest opcode )
    bl bc_comma                         @ ( orig dest opcode -- )

@ Resolve leave-sys and decrement leave_lvl
@ r0    leave_lvl address
@ r1    leave_lvl decremented
@ tos   leave_lvl
    PUSH_TOS
    ldr r0, =leave_lvl
    ldrb tos, [r0]                      @ ( -- leave_lvl )
    subs r1, tos, #1
    strb r1, [r0]
    bl csp_comma                        @ ( leave_lvl -- ) ( C: leave-sys -- )

@ Return
    pop {pc}
.ltorg
.endif

@ ------------------------------------------------------------------------------
@ ,
@ ( x -- )
@ Reserve one cell of data space and store x in the cell. If the data-space
@ pointer is aligned when , begins execution, it will remain aligned when ,
@ finishes execution. An ambiguous condition exists if the data-space pointer
@ is not aligned prior to execution of ,.
@ ------------------------------------------------------------------------------
.if ENABLE_COMMA == 1
WORD FLAG_INTERPRET_COMPILE, ",", comma
    ldr r0, =data_begin
    ldr r1, [r0]
    str tos, [r1], #4                   @ Write x to address in data_begin
    str r1, [r0]                        @ Update address in data_begin
    DROP                                @ ( x -- )
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ -
@ ( n1 n2 -- n3 )
@ Subtract n2 from n1, giving the difference n3
@ ------------------------------------------------------------------------------
.if ENABLE_MINUS == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "-", minus
    ldmia dsp!, {r0}                    @ ( n1 n2 -- n2 )
    subs tos, r0, tos                   @ ( n2 -- n3 )
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ /
@ ( n1 n2 -- n3 )
@ Divide n1 by n2, giving the single-cell quotient n3
@ ------------------------------------------------------------------------------
.if ENABLE_DIV == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "/", div
    ldmia dsp!, {r0}                    @ ( n1 n2 -- n2 )
    sdiv tos, r0, tos                   @ ( n2 -- n3 )
    bx lr
.endif

.if ENABLE_DIV_MOD == 1
WORD FLAG_SKIP, "/mod", div_mod
.endif

@ ------------------------------------------------------------------------------
@ 0<
@ ( n -- flag )
@ flag is true if and only if n is less than zero.
@ ------------------------------------------------------------------------------
.if ENABLE_ZERO_LESS == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "0<", zero_less
    cmp tos, #0
    ite lt
    movlt tos, #-1                      @ ( n -- flag )
    movge tos, #0                       @ ( n -- flag )
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ 0<
@ ( n -- flag )
@ flag is true if and only if x is equal to zero.
@ ------------------------------------------------------------------------------
.if ENABLE_ZERO_EQUAL == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "0=", zero_equal
    cmp tos, #0
    ite eq
    moveq tos, #-1                      @ ( n -- flag )
    movne tos, #0                       @ ( n -- flag )
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ 1+
@ ( n1 -- n2 )
@ Add one (1) to n1 giving the sum n2.
@ ------------------------------------------------------------------------------
.if ENABLE_ONE_PLUS == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "1+", one_plus
    adds tos, #1
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ 1-
@ ( n1 -- n2 )
@ Subtract one (1) from n1 giving the difference n2.
@ ------------------------------------------------------------------------------
.if ENABLE_ONE_MINUS == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "1-", one_minus
    subs tos, #1
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ 2!
@ ( x1 x2 a-addr -- )
@ Store the cell pair x1 x2 at a-addr, with x2 at a-addr and x1 at the next
@ consecutive cell. It is equivalent to the sequence swap over ! cell+ !.
@ ------------------------------------------------------------------------------
.if ENABLE_TWO_STORE == 1
WORD FLAG_INTERPRET_COMPILE, "2!", two_store
    ldmia dsp!, {r0, r1}                @ ( x1 x2 a-addr -- a-addr )
    str r0, [tos]
    str r1, [tos, #4]
    DROP                                @ ( a-addr -- )
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ 2*
@ ( x1 -- x2 )
@ x2 is the result of shifting x1 one bit toward the most-significant bit,
@ filling the vacated least-significant bit with zero.
@ ------------------------------------------------------------------------------
.if ENABLE_TWO_TIMES == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "2*", two_times
    lsl tos, #1
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ 2/
@ ( x1 -- x2 )
@ x2 is the result of shifting x1 one bit toward the least-significant bit,
@ leaving the most-significant bit unchanged.
@ ------------------------------------------------------------------------------
.if ENABLE_TWO_DIV == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "2/", two_div
    asr tos, #1
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ 2@
@ ( a-addr -- x1 x2 )
@ Fetch the cell pair x1 x2 stored at a-addr. x2 is stored at a-addr and x1 at
@ the next consecutive cell. It is equivalent to the sequence dup cell+ @ swap
@ @.
@ ------------------------------------------------------------------------------
.if ENABLE_TWO_FETCH == 1
WORD FLAG_INTERPRET_COMPILE, "2@", two_fetch
    ldr r0, [tos, #4]
    str r0, [dsp, #-4]!
    ldr tos, [tos]
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ 2drop
@ ( x1 x2 -- )
@ Drop cell pair x1 x2 from the stack.
@ ------------------------------------------------------------------------------
.if ENABLE_TWO_DROP == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "2drop", two_drop
    TWO_DROP
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ 2dup
@ ( x1 x2 -- x1 x2 x1 x2 )
@ Duplicate cell pair x1 x2.
@ ------------------------------------------------------------------------------
.if ENABLE_TWO_DUP == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "2dup", two_dup
    TWO_DUP
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ 2over
@ ( x1 x2 x3 x4 -- x1 x2 x3 x4 x1 x2 )
@ Copy cell pair x1 x2 to the top of the stack.
@ ------------------------------------------------------------------------------
.if ENABLE_TWO_OVER == 1
WORD FLAG_INTERPRET_COMPILE, "2over", two_over
    TWO_OVER
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ 2swap
@ ( x1 x2 x3 x4 -- x3 x4 x1 x2 )
@ Exchange the top two cell pairs.
@ ------------------------------------------------------------------------------
.if ENABLE_TWO_SWAP == 1
WORD FLAG_INTERPRET_COMPILE, "2swap", two_swap
    TWO_SWAP
    bx lr
.endif

@ ------------------------------------------------------------------------------
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
@ ------------------------------------------------------------------------------
.if ENABLE_COLON == 1
WORD FLAG_INTERPRET_COMPILE, ":", colon
    push {lr}

@ TODO check stack balance? (maybe even return stack?)

@ Create word
    bl word_comma                       @ ( -- )

@ Change flags set by create and make current definition unfindable
@ r0    link
@ r1    flags
    ldr r0, =link
    ldr r0, [r0]
    movs r1, #FLAG_SKIP
    strb r1, [r0, #4]

@ push {lr}
    PUSH_INT16 #0xB500                  @ ( -- opcode )
    bl h_comma                          @ ( opcode -- )

@ Enter compilation state
    bl bracket_right                    @ ( -- )

@ Return
    pop {pc}
.ltorg
.endif

@ ------------------------------------------------------------------------------
@ ;
@ ( colon-sys -- )
@ Append the run-time semantics below to the current definition. End the current
@ definition, allow it to be found in the dictionary and enter interpretation
@ state, consuming colon-sys. If the data-space pointer is not aligned, reserve
@ enough data space to align it.
@
@ ( -- ) ( R: nest-sys -- )
@ Return to the calling definition specified by nest-sys.
@ ------------------------------------------------------------------------------
.if ENABLE_SEMI == 1
WORD FLAG_COMPILE_IMMEDIATE, ";", semi
    push {lr}

// TODO check stack balance? (maybe even return stack?)

@ Write return
    bl exit                             @ ( -- )

@ Change flags set by : and make definition findable
@ r0    link
@ r1    flags
    ldr r0, =link
    ldr r0, [r0]
    movs r1, #FLAG_INTERPRET_COMPILE
    strb r1, [r0, #4]

@ Enter interpretation state
    bl bracket_left                     @ ( -- )

@ Return
    pop {pc}
.endif

@ ------------------------------------------------------------------------------
@ <
@ ( n1 n2 -- flag )
@ Flag is true if and only if n1 is less than n2.
@ ------------------------------------------------------------------------------
.if ENABLE_LESS == 1
WORD FLAG_INTERPRET_COMPILE & FOLDS_2, "<", less
    ldmia dsp!, {r0}                    @ ( n1 n2 -- n2 )
    cmp r0, tos
    ite lt
    movlt tos, #-1                      @ ( n2 -- flag )
    movge tos, #0                       @ ( n2 -- flag )
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ =
@ ( x1 x2 -- flag )
@ Flag is true if and only if x1 is bit-for-bit the same as x2.
@ ------------------------------------------------------------------------------
.if ENABLE_EQUAL == 1
WORD FLAG_INTERPRET_COMPILE & FOLDS_2, "=", equal
    ldmia dsp!, {r0}                    @ ( x1 x2 -- x2 )
    cmp r0, tos
    ite eq
    moveq tos, #-1                      @ ( x2 -- flag )
    movne tos, #0                       @ ( x2 -- flag )
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ >
@ ( n1 n2 -- flag )
@ Flag is true if and only if n1 is greater than n2.
@ ------------------------------------------------------------------------------
.if ENABLE_MORE == 1
WORD FLAG_INTERPRET_COMPILE & FOLDS_2, ">", more
    ldmia dsp!, {r0}                    @ ( n1 n2 -- n2 )
    cmp r0, tos
    ite gt
    movgt tos, #-1                      @ ( n2 -- flag )
    movle tos, #0                       @ ( n2 -- flag )
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ ( xt -- a-addr )
@ a-addr is the data-field address corresponding to xt. An ambiguous condition
@ exists if xt is not for a word defined via create.
@ ------------------------------------------------------------------------------
.if ENABLE_TO_BODY == 1
WORD FLAG_INTERPRET_COMPILE & FOLDS_1, ">body", to_body
    adds tos, #14                       @ align(xt + 14, 4)
    P2ALIGN2 align=tos, scratch=r12     @ ( xt -- a-addr )
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ >in
@ ( -- a-addr )
@ a-addr is the address of a cell containing the offset in characters from the
@ start of the input buffer to the start of the parse area.
@ ------------------------------------------------------------------------------
.if ENABLE_TO_IN == 1
WORD FLAG_INTERPRET_COMPILE, ">in", to_in
    PUSH_TOS
    ldr tos, =in
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ >r
@ (    x --   )
@ ( R:   -- x )
@ Move x to the return stack.
@ ------------------------------------------------------------------------------
.if ENABLE_TO_R == 1
WORD FLAG_COMPILE & FLAG_INLINE, ">r", to_r
    TO_R
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ >r
@ ( x -- 0 | x x )
@ Duplicate x if it is non-zero.
@ ------------------------------------------------------------------------------
.if ENABLE_Q_DUP == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "?dup", q_dup
    Q_DUP
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ @
@ ( a-addr -- x )
@ x is the value stored at a-addr.
@ ------------------------------------------------------------------------------
.if ENABLE_FETCH == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE, "@", fetch
    ldr tos, [tos]
    bx lr
.endif

.if ENABLE_ABORT == 1
WORD FLAG_SKIP, "abort"
.endif

.if ENABLE_ABORT_Q == 1
WORD FLAG_SKIP, "abort\"", abort_q
.endif

@ ------------------------------------------------------------------------------
@ abs
@ ( n -- u )
@ u is the absolute value of n.
@ ------------------------------------------------------------------------------
.if ENABLE_ABS == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "abs"
    asrs r0, tos, #31
    adds tos, r0
    eors tos, r0
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ align
@ ( -- )
@ If the data-space pointer is not aligned, reserve enough space to align it.
@ ------------------------------------------------------------------------------
.if ENABLE_ALIGN == 1
WORD FLAG_INTERPRET_COMPILE, "align"
    ldr r0, =data_begin
    ldr r1, [r0]
    P2ALIGN1 align=r1, scratch=r12
    str r1, [r0]
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ aligned
@ ( addr -- a-addr )
@ a-addr is the first aligned address greater than or equal to addr.
@ ------------------------------------------------------------------------------
.if ENABLE_ALIGNED == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "aligned"
    P2ALIGN1 align=tos, scratch=r12
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ allot
@ ( n -- )
@ If n is greater than zero, reserve n address units of data space. If n is less
@ than zero, release | n | address units of data space. If n is zero, leave the
@ data-space pointer unchanged.
@ If the data-space pointer is aligned and n is a multiple of the size of a cell
@ when allot begins execution, it will remain aligned when allot finishes
@ execution.
@ If the data-space pointer is character aligned and n is a multiple of the size
@ of a character when allot begins execution, it will remain character aligned
@ when allot finishes execution.
@ ------------------------------------------------------------------------------
.if ENABLE_ALLOT == 1
WORD FLAG_INTERPRET_COMPILE, "allot"
    ldr r0, =data_begin
    ldr r1, [r0]
    adds r1, tos
    str r1, [r0]
    DROP                                @ ( n -- )
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ and
@ ( x1 x2 -- x3 )
@ x3 is the bit-by-bit logical "and" of x1 with x2.
@ ------------------------------------------------------------------------------
.if ENABLE_AND == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "and"
    ldmia dsp!, {r0}                    @ ( x1 x2 -- x2 )
    ands tos, r0                        @ ( x2 -- x3 )
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ base
@ ( -- a-addr )
@ a-addr is the address of a cell containing the current number-conversion radix
@ {{2...36}}.
@ ------------------------------------------------------------------------------
.if ENABLE_BASE == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE, "base"
    PUSH_TOS
    ldr tos, =radix                     @ ( -- a-addr )
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ begin
@ ( -- dest )
@ Put the next location for a transfer of control, dest, onto the stack. Append
@ the run-time semantics given below to the current definition.
@
@ ( -- )
@ Continue execution.
@ ------------------------------------------------------------------------------
.if ENABLE_BEGIN == 1
WORD FLAG_COMPILE_IMMEDIATE, "begin"
    push {lr}

@ dest
    bl here                             @ ( -- dest )

@ Return
    pop {pc}
.endif

@ ------------------------------------------------------------------------------
@ c!
@ ( char c-addr -- )
@ Store char at c-addr. When character size is smaller than cell size, only the
@ number of low-order bits corresponding to character size are transferred.
@ ------------------------------------------------------------------------------
.if ENABLE_C_STORE == 1
WORD FLAG_INTERPRET_COMPILE, "c!", c_store
    ldrb r0, [dsp], #4                  @ ( char c-addr -- c-addr )
    strb r0, [tos]
    DROP                                @ ( c-addr -- )
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ c,
@ ( char -- )
@ Reserve space for one character in the data space and store char in the space.
@ If the data-space pointer is character aligned when c, begins execution, it
@ will remain character aligned when c, finishes execution. An ambiguous
@ condition exists if the data-space pointer is not character-aligned prior to
@ execution of c,.
@ ------------------------------------------------------------------------------
.if ENABLE_C_COMMA == 1
WORD FLAG_INTERPRET_COMPILE, "c,", c_comma
    ldr r0, =data_begin
    ldr r1, [r0]
    strb tos, [r1], #1                  @ Write char to address in data_begin
    str r1, [r0]                        @ Update address in data_begin
    DROP                                @ ( char -- )
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ c@
@ ( c-addr -- char )
@ Fetch the character stored at c-addr. When the cell size is greater than
@ character size, the unused high-order bits are all zeroes.
@ ------------------------------------------------------------------------------
.if ENABLE_C_FETCH == 1
WORD FLAG_INTERPRET_COMPILE, "c@", c_fetch
    ldrb tos, [tos]
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ cell+
@ ( a-addr1 -- a-addr2 )
@ Add the size in address units of a cell to a-addr1, giving a-addr2.
@ ------------------------------------------------------------------------------
.if ENABLE_CELL_PLUS == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "cell+", cell_plus
    adds tos, #4
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ cells
@ ( n1 -- n2 )
@ n2 is the size in address units of n1 cells.
@ ------------------------------------------------------------------------------
.if ENABLE_CELLS == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "cells", cells
    lsl tos, #2
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ char+
@ ( c-addr1 -- c-addr2 )
@ Add the size in address units of a character to c-addr1, giving c-addr2.
@ ------------------------------------------------------------------------------
.if ENABLE_CHAR_PLUS == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "char+", char_plus
    adds tos, #1
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ chars
@ ( n1 -- n2 )
@ n2 is the size in address units of n1 characters.
@ ------------------------------------------------------------------------------
.if ENABLE_CHARS == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "chars"
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ constant
@ ( source: "<spaces>name" -- )
@ (                      x -- )
@ Skip leading space delimiters. Parse name delimited by a space. Create a
@ definition for name with the execution semantics defined below.
@ name is referred to as a "constant".
@
@ ( -- x )
@ Place x on the stack.
@ ------------------------------------------------------------------------------
.if ENABLE_CONSTANT == 1
WORD FLAG_INTERPRET_COMPILE, "constant"
    push {lr}

@ Create word
    bl word_comma                       @ ( -- )

@ Write literal
    bl literal                          @ ( x -- )

@ bx lr
    PUSH_INT16 #0x4770                  @ ( -- opcode )
    bl h_comma                          @ ( opcode -- )

@ Return
    pop {pc}
.endif

@ ------------------------------------------------------------------------------
@ create
@ ( source: "<spaces>name" -- )
@ Skip leading space delimiters. Parse name delimited by a space. Create a
@ definition for name with the execution semantics defined below. If the
@ data-space pointer is not aligned, reserve enough data space to align it. The
@ new data-space pointer defines name's data field. create does not allocate
@ data space in name's data field.
@
@ ( -- a-addr )
@ a-addr is the address of name's data field. The execution semantics of name
@ may be extended by using does>.
@ ------------------------------------------------------------------------------
.if ENABLE_CREATE == 1
WORD FLAG_INTERPRET_COMPILE, "create"
    push {lr}

@ Create word
    bl word_comma                       @ ( -- )

@ push {lr}
    PUSH_INT16 #0xB500                  @ ( -- opcode )
    bl h_comma                          @ ( opcode -- )

@ str tos, [dsp, #-4]!
    PUSH_TOS
    ldr tos, =0x6D04F847                @ ( -- opcode )
    bl comma                            @ ( opcode -- )

@ Additions to pc can only be 4 byte aligned, add nop if necessary
@ nop
    bl here
    ands tos, #3
    beq 1f
        PUSH_INT16 #0xBF00              @ ( -- opcode )
        bl h_comma                      @ ( opcode -- )

@ add tos, pc, #4
1:  PUSH_TOS
    ldr tos, =0x0604F20F                @ ( -- opcode )
    bl comma                            @ ( opcode -- )

@ pop {pc}
    PUSH_INT16 #0xBD00                  @ ( -- opcode )
    bl h_comma                          @ ( opcode -- )

@ Allot enough space to allow does> changing bx lr to b
    PUSH_INT8 #2
    bl allot

@ Return
    DROP
    pop {pc}
.endif

@ ------------------------------------------------------------------------------
@ decimal
@ ( -- )
@ Set contents of radix to ten.
@ ------------------------------------------------------------------------------
.if ENABLE_DECIMAL == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE, "decimal"
    ldr r0, =radix
    movs r1, #10
    str r1, [r0]
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ depth
@ ( -- +n )
@ +n is the number of single-cell values contained in the data stack before +n
@ was placed on the stack.
@ ------------------------------------------------------------------------------
.if ENABLE_DEPTH == 1
WORD FLAG_INTERPRET_COMPILE, "depth"
    DEPTH
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ do
@ ( -- dest )
@ Place dest onto the stack. Append the run-time semantics given below to the
@ current definition. The semantics are incomplete until resolved by a consumer
@ of dest such as loop.
@
@ (    n1 n2 --          )
@ ( R:       -- loop-sys )
@ Set up loop control parameters with index n2 and limit n1. An ambiguous
@ condition exists if n1 and n2 are not both the same type. Anything already on
@ the return stack becomes unavailable until the loop-control parameters are
@ discarded.
@ ------------------------------------------------------------------------------
.if ENABLE_DO == 1
WORD FLAG_COMPILE_IMMEDIATE, "do"
    push {lr}

@ Do consumes n1 and n2 and pushes it onto the return stack
@ movs r0, tos
@ ldmia dsp!, {r1, tos}
    PUSH_TOS
    ldr tos, =0xCF420030                @ ( -- opcode )
    bl comma                            @ ( opcode -- )

@ push {r0, r1}
    PUSH_INT16 #0xB403                  @ ( -- opcode )
    bl h_comma                          @ ( opcode -- )

@ Dest
    bl here                             @ ( -- dest )

@ Increment leave_lvl
@ r0    leave_lvl address
@ r1    leave_lvl
    ldr r0, =leave_lvl
    ldrb r1, [r0]
    adds r1, #1
    strb r1, [r0]

@ Return
    pop {pc}                          
.endif

@ ------------------------------------------------------------------------------
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
@ ------------------------------------------------------------------------------
.if ENABLE_DOES == 1
WORD FLAG_COMPILE, "does>", does
@ Get address of bx lr of latest definition's create
@ tos   bx lr address
    PUSH_TOS
    ldr tos, =link
    ldr tos, [tos]
    ldrb r1, [tos, #5]                  @ c-addr
    adds tos, r1
    adds tos, #16                       @ align(link + c-addr + 12, 4)
    P2ALIGN2 align=tos, scratch=r12
    ldrh r0, [tos]

@ Replace bx lr of create with branch to code after does>
@ tos   bx lr address
@ lr    does> link address
    PUSH_REGS lr                        @ ( -- orig dest )
    bl b_comma                          @ ( orig dest -- )

@ Return
    pop {pc}
.endif

@ ------------------------------------------------------------------------------
@ drop
@ ( x -- )
@ Remove x from the stack.
@ ------------------------------------------------------------------------------
.if ENABLE_DROP == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "drop"
    DROP
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ dup
@ ( x -- x x )
@ Duplicate x.
@ ------------------------------------------------------------------------------
.if ENABLE_DUP == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "dup"
    DUP
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ else
@ ( orig1 -- orig2 )
@ Put the location of a new unresolved forward reference orig2 and it's function
@ pointer onto the stack. Append the run-time semantics given below to the
@ current definition. The semantics will be incomplete until orig2 is resolved
@ (e.g., by then). Resolve the forward reference orig1 using the location
@ following the appended run-time semantics.
@
@ ( -- )
@ Continue execution at the location given by the resolution of orig2.
@ ------------------------------------------------------------------------------
.if ENABLE_ELSE == 1
WORD FLAG_COMPILE_IMMEDIATE, "else"
    push {lr}

@ orig2
    bl here                             @ ( orig1 -- orig1 orig2 )
    orrs tos, #1                        @ Signal to then to compile an unconditional branch

@ Reserve space for orig2
    PUSH_INT8 #4
    bl allot

@ Resolve orig1
    SWAP                                @ ( orig1 orig2 -- orig2 orig1 )
    bl here                             @ ( orig2 orig1 -- orig2 orig1 dest )
    PUSH_TOS                            @ beq
    ldr tos, =0xF0008000                @ ( orig2 orig1 dest -- orig2 orig1 dest opcode )
    bl bc_comma                         @ ( orig2 orig1 dest opcode -- orig2 )

@ Return
    pop {pc}
.endif

@ ------------------------------------------------------------------------------
@ execute
@ ( i * x xt -- j * x )
@ Remove xt from the stack and perform the semantics identified by it. Other
@ stack effects are due to the word executed.
@ ------------------------------------------------------------------------------
.if ENABLE_EXECUTE == 1
WORD FLAG_INTERPRET, "execute"
    POP_REGS r0
    mov pc, r0
.endif

@ ------------------------------------------------------------------------------
@ exit
@ (             -- )
@ ( R: nest-sys -- )
@ Return control to the calling definition specified by nest-sys. Before
@ executing exit within a do-loop, a program shall discard the loop-control
@ parameters by executing unloop.
@ ------------------------------------------------------------------------------
.if ENABLE_EXIT == 1
WORD FLAG_COMPILE_IMMEDIATE, "exit"
    push {lr}

@ TODO needs to check if unloop needs to be called?!

@ pop {pc}
    PUSH_INT16 #0xBD00                  @ ( -- opcode )
    bl h_comma                          @ ( opcode -- )

@ Return
    pop {pc}
.endif

.if ENABLE_FILL == 1
WORD FLAG_SKIP, "fill"
.endif

@ ------------------------------------------------------------------------------
@ find
@ ( token-addr token-u -- token-addr false | xt flags )
@ Find the definition named in the counted string at c-addr. If the definition
@ is not found, return token-addr and zero. If the definition is found, return
@ its execution token xt and its flags.
@ ------------------------------------------------------------------------------
.if ENABLE_FIND == 1
WORD FLAG_INTERPRET_COMPILE, "find"
    push {r4}

@ Search
@ r0    link
@ r1    flags
    ldr r0, =link
1:  ldr r0, [r0]                        @ Link
    cmp r0, #LINK_INVALID               @ link - LINK_INVALID
    beq 3f                              @ Goto found nothing
        ldrb r1, [r0, #4]               @ Flags
        cmp r1, #FLAG_SKIP              @ Check if word should be skipped
        beq 1b                          @ Goto search

@ Length compare
@ r0    link
@ r1    u
@ r2    c-addr incremented
@ r3    token-addr incremented
@ tos   token-u
    ldrb r1, [r0, #5]                   @ u
    cmp r1, tos                         @ u - token-u
    bne 1b                              @ Goto search
        adds r2, r0, #6                 @ c-addr incremented
        ldr r3, [dsp]                   @ token-addr incremented

@ Bytewise compare
@ r1    u
@ r2    c-addr incremented
@ r3    token-addr incremented
@ r4    character
@ r12   character
2:      ldrb r4, [r2], #1               @ Character
        ldrb r12, [r3], #1
        cmp r4, r12                     @ character - character
        bne 1b                          @ Goto search
            subs r1, #1
            bhi 2b                      @ Goto bytewise compare
            b 4f                        @ Goto found something

@ Found nothing
3:  movs tos, #0                        @ ( token-addr token-u -- token-addr false )
    b 6f

@ Found something
4:  P2ALIGN1 align=r2, scratch=r12      @ xt is at next 2-byte aligned address
    str r2, [dsp]
    ldrb tos, [r0, #4]                  @ ( token-addr token-u -- xt flags )

@ Return
6:  pop {r4}
    bx lr
.endif

.if ENABLE_FM_DIV_MOD == 1
WORD FLAG_SKIP, "fm/mod", fm_div_mod
.endif

@ ------------------------------------------------------------------------------
@ here
@ ( -- addr )
@ addr is the data-space pointer.
@ ------------------------------------------------------------------------------
.if ENABLE_HERE == 1
WORD FLAG_INTERPRET_COMPILE, "here"
    PUSH_TOS
    ldr tos, =data_begin
    ldr tos, [tos]
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ i
@ (             -- n        )
@ ( R: loop-sys -- loop-sys )
@ n is a copy of the current (innermost) loop index. An ambiguous condition
@ exists if the loop control parameters are unavailable.
@ ------------------------------------------------------------------------------
.if ENABLE_I == 1
WORD FLAG_COMPILE_IMMEDIATE, "i"
    push {lr}

@ str tos, [dsp, #-4]!
    PUSH_TOS
    ldr tos, =0x6D04F847                @ ( -- opcode )
    bl comma                            @ ( opcode -- )

@ ldr tos, [sp]
    PUSH_INT16 #0x9E00                  @ ( -- opcode )
    bl h_comma                          @ ( opcode -- )

@ Return
    pop {pc}
.endif

@ ------------------------------------------------------------------------------
@ if
@ ( -- orig1 )
@ Put the location of a new unresolved forward reference orig1 and it's function
@ pointer onto the stack. Append the run-time semantics given below to the
@ current definition. The semantics are incomplete until orig1 is resolved,
@ e.g., by then or else.
@
@ ( x -- )
@ If all bits of x are zero, continue execution at the location specified by the
@ resolution of orig.
@ ------------------------------------------------------------------------------
.if ENABLE_IF == 1
WORD FLAG_COMPILE_IMMEDIATE, "if"
    push {lr}

@ If needs to consume its own flag
@ movs r0, tos
@ ldmia dsp!, {tos}
    PUSH_TOS
    ldr tos, =0xCF400030                @ ( -- opcode )
    bl comma                            @ ( opcode -- )

@ cmp r0, #0
    PUSH_INT16 #0x2800                  @ ( -- opcode )
    bl h_comma                          @ ( opcode -- )

@ orig1
    bl here                             @ ( -- orig1 )

@ Reserve space for orig1
    PUSH_INT8 #4
    bl allot

@ Return
    pop {pc}
.ltorg
.endif

.if ENABLE_IMMEDIATE == 1
WORD FLAG_SKIP, "immediate"
.endif

@ ------------------------------------------------------------------------------
@ invert
@ ( x1 -- x2 )
@ Invert all bits of x1, giving its logical inverse x2.
@ ------------------------------------------------------------------------------
.if ENABLE_INVERT == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "invert"
    mvns tos, tos
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ j
@ (                        -- n                   )
@ ( R: loop-sys1 loop-sys2 -- loop-sys1 loop-sys2 )
@ n is a copy of the next-outer loop index. An ambiguous condition exists if the
@ loop control parameters of the next-outer loop, loop-sys1, are unavailable.
@ ------------------------------------------------------------------------------
.if ENABLE_J == 1
WORD FLAG_COMPILE_IMMEDIATE, "j"
    push {lr}

@ str tos, [dsp, #-4]!
    PUSH_TOS
    ldr tos, =0x6D04F847                @ ( -- opcode )
    bl comma                            @ ( opcode -- )

@ ldr tos, [sp, #8]
    PUSH_INT16 #0x9E02                  @ ( -- opcode )
    bl h_comma                          @ ( opcode -- )

@ Return
    pop {pc}
.endif

@ ------------------------------------------------------------------------------
@ leave
@ ( C:          -- leave-sys )
@ ( R: loop-sys --           )
@ Discard the current loop control parameters. An ambiguous condition exists if
@ they are unavailable. Continue execution immediately following the innermost
@ syntactically enclosing do...loop or do...+loop.
@ ------------------------------------------------------------------------------
.if ENABLE_LEAVE == 1
WORD FLAG_COMPILE_IMMEDIATE, "leave"
    push {lr}

@ Unloop
    bl unloop                           @ ( R: loop-sys -- )

@ Leave-sys
    bl here                             @ ( -- leave-sys )

@ Reserve space for leave-sys
    PUSH_INT8 #4                        @ Reserve space for branch instruction
    bl allot

@ Reverse push leave-sys onto the stack
@ r0    csp address
@ r1    csp
@ r2    leave_lvl
@ tos   leave-sys
    ldr r0, =csp
    ldr r1, [r0]
    ldr r2, =leave_lvl
    ldrb r2, [r2]
    strd r2, tos, [r1], #8              @ ( C: -- leave-sys )
    str r1, [r0]
    DROP                                @ ( leave-sys -- )

@ Return
    pop {pc}
.endif

@ ------------------------------------------------------------------------------
@ literal
@ ( x -- )
@ Append the run-time semantics given below to the current definition.
@ Compiles code for a literal x in the data-space. This could either be:
@ movs (2 bytes)
@ movs.w (4 bytes)
@ movw movt (8 bytes)
@
@ ( -- x )
@ Place x on the stack.
@ ------------------------------------------------------------------------------
.if ENABLE_LITERAL == 1
WORD FLAG_COMPILE_IMMEDIATE, "literal", literal
    push {lr}

@ str tos, [dsp, #-4]!
    PUSH_TOS
    ldr tos, =0x6D04F847                @ ( -- opcode )
    bl comma                            @ ( opcode -- )

@ movs (t1)
@ tos   x
    cmp tos, #0xFF
    bhi 1f                              @ Goto movs (t2)
        orrs tos, #0x2600               @ Opcode template (can be ored directly into tos)
        bl h_comma                      @ ( opcode -- )
            b 6f                        @ Goto return

@ movs (t2)
@ Pattern 0x00XY00XY
@ r0    bottom
@ r1    top
@ tos   x
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
        bl rev_comma                    @ ( opcode -- )
            b 6f                        @ Goto return

@ Pattern 0xXY00XY00
@ r0    bottom
@ r1    top
@ tos   x
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
        bl rev_comma                    @ ( opcode -- )
            b 6f                        @ Goto return

@ Pattern 0xXYXYXYXY
@ r0    rotated copy of tos
@ tos   x
1:  movs r0, tos
    rors r0, #16
    cmp r0, tos
    bne 1f                              @ Goto pattern 0x00000XY0
        rors r0, #8
        cmp r0, tos
        bne 1f                          @ Goto pattern 0x00000XY0

        ldr tos, =0xF05F3600            @ Opcode template
        orrs tos, tos, r0, lsr #24      @ Or imm8 into template
        bl rev_comma                    @ ( opcode -- )
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

            bl rev_comma                @ ( opcode -- )

            b 6f                        @ Goto return

// TODO mvn might offer some patterns as well?

@ movw movt
@ r0    bottom | top
@ r1    intermediate
@ r2    x
@ tos   opcode
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
    bl rev_comma                        @ ( opcode -- )
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

    bl rev_comma                        @ ( opcode -- )

@ Return
6:  pop {pc}
.ltorg
.endif

@ ------------------------------------------------------------------------------
@ loop
@ (         dest -- )
@ ( C: leave-sys -- )
@ Append the run-time semantics given below to the current definition. Resolve
@ the destination of all unresolved occurrences of leave between the location
@ given by leave-sys and the next location for a transfer of control, to execute
@ the words following the loop.
@
@ (              --             )
@ ( R: loop-sys1 -- | loop-sys2 )
@ An ambiguous condition exists if the loop control parameters are unavailable.
@ Add one to the loop index. If the loop index is then equal to the loop limit,
@ discard the loop parameters and continue execution immediately following the
@ loop. Otherwise continue execution at the beginning of the loop.
@ ------------------------------------------------------------------------------
.if ENABLE_LOOP == 1
WORD FLAG_COMPILE_IMMEDIATE, "loop"
    push {lr}

@ pop {r0, r1}
@ adds r0, #1
    PUSH_TOS
    ldr tos, =0x3001BC03                @ ( -- opcode )
    bl comma                            @ ( opcode -- )

@ cmp r0, r1
@ it ne
    PUSH_TOS
    ldr tos, =0xBF184288                @ ( -- opcode )
    bl comma                            @ ( opcode -- )

@ pushne {r0, r1}
    PUSH_INT16 #0xB403                  @ ( -- opcode )
    bl h_comma                          @ ( opcode -- )

@ Resolve dest
    bl here                             @ ( -- dest orig )
    SWAP                                @ ( dest orig -- orig dest )
    PUSH_TOS                            @ beq
    ldr tos, =0xF0408000                @ ( orig dest -- orig dest opcode )
    bl bc_comma                         @ ( orig dest opcode -- )

@ Resolve leave-sys and decrement leave_lvl
@ r0    leave_lvl address
@ r1    leave_lvl decremented
@ tos   leave_lvl
    PUSH_TOS
    ldr r0, =leave_lvl
    ldrb tos, [r0]                      @ ( -- leave_lvl )
    subs r1, tos, #1
    strb r1, [r0]
    bl csp_comma                        @ ( leave_lvl -- ) ( C: leave-sys -- )

@ Return
    pop {pc}
.ltorg
.endif

@ ------------------------------------------------------------------------------
@ lshift
@ ( x1 u -- x2 )
@ Perform a logical left shift of u bit-places on x1, giving x2. Put zeroes into
@ the least significant bits vacated by the shift. An ambiguous condition exists
@ if u is greater than or equal to the number of bits in a cell.
@ ------------------------------------------------------------------------------
.if ENABLE_LSHIFT == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "lshift"
    ldmia dsp!, {r0}
    lsls tos, r0, tos
    bx lr
.endif

.if ENABLE_M_TIMES == 1
WORD FLAG_SKIP, "m*", m_times
.endif

@ ------------------------------------------------------------------------------
@ max
@ ( n1 n2 -- n3 )
@ n3 is the greater of n1 and n2.
@ ------------------------------------------------------------------------------
.if ENABLE_MAX == 1
WORD FLAG_INTERPRET_COMPILE & FOLDS_2, "max"
    ldmia dsp!, {r0}
    cmp r0, tos
    it gt
    movgt tos, r0
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ min
@ ( n1 n2 -- n3 )
@ n3 is the lesser of n1 and n2.
@ ------------------------------------------------------------------------------
.if ENABLE_MIN == 1
WORD FLAG_INTERPRET_COMPILE & FOLDS_2, "min"
    ldmia dsp!, {r0}
    cmp r0, tos
    it lt
    movlt tos, r0
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ mod
@ ( n1 n2 -- n3 )
@ Divide n1 by n2, giving the single-cell remainder n3. An ambiguous condition
@ exists if n2 is zero. If n1 and n2 differ in sign, the implementation-defined
@ result returned will be the same as that returned by either the phrase >r s>d
@ r> fm/mod drop or the phrase >r s>d r> sm/rem drop.
@ ------------------------------------------------------------------------------
.if ENABLE_MOD == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "mod"
    ldr r0, [dsp], #4
    sdiv    r1, r0, tos
    mls     tos, r1, tos, r0
    bx lr
.endif

.if ENABLE_MOVE == 1
WORD FLAG_SKIP, "move"
.endif

@ ------------------------------------------------------------------------------
@ negate
@ ( n1 -- n2 )
@ n2 is the negated value of n1.
@ ------------------------------------------------------------------------------
.if ENABLE_NEGATE == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "negate"
    rsbs tos, tos, #0
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ or
@ ( x1 x2 -- x3 )
@ x3 is the bit-by-bit inclusive-or of x1 with x2.
@ ------------------------------------------------------------------------------
.if ENABLE_OR == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "or"
    ldmia dsp!, {r0}
    orrs tos, r0
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ over
@ ( x1 x2 -- x1 x2 x1 )
@ Place a copy of x1 on top of the stack.
@ ------------------------------------------------------------------------------
.if ENABLE_OVER == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "over"
    OVER
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ postpone
@ ( "<spaces>name" -- )
@ Skip leading space delimiters. Parse name delimited by a space. Find name.
@ Append the compilation semantics of name to the current definition. An
@ ambiguous condition exists if name is not found.
@ ------------------------------------------------------------------------------
.if ENABLE_POSTPONE == 1
WORD FLAG_COMPILE_IMMEDIATE, "postpone"
    push {lr}
    bl tick
    bl compile_comma
    pop {pc}
.endif

.if ENABLE_QUIT == 1
WORD FLAG_SKIP, "quit"
.endif

@ ------------------------------------------------------------------------------
@ >r
@ (      -- x )
@ ( R: x --   )
@ Move x from the return stack to the data-stack.
@ ------------------------------------------------------------------------------
.if ENABLE_R_FROM == 1
WORD FLAG_COMPILE & FLAG_INLINE, "r>", r_from
    R_FROM
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ r@
@ (      -- x )
@ ( R: x -- x )
@ Copy x from the return stack to the data stack.
@ ------------------------------------------------------------------------------
.if ENABLE_R_FETCH == 1
WORD FLAG_COMPILE & FLAG_INLINE, "r@", r_fetch
    PUSH_TOS
    ldr tos, [sp]
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ recurse
@ ( -- )
@ Append the execution semantics of the current definition to the current
@ definition. An ambiguous condition exists if recurse appears in a definition
@ after does>.
@ ------------------------------------------------------------------------------
.if ENABLE_RECURSE == 1
WORD FLAG_COMPILE_IMMEDIATE, "recurse"
    push {lr}

@ xt of latest definition
@ r0    link address
@ r1    link
@ r2    c-addr
@ tos   xt
    PUSH_TOS
    ldr r0, =link
    ldr r1, [r0]
    ldrb r2, [r1, #5]                   @ c-addr
    adds tos, r1, r2
    adds tos, #4+2                      @ align(link + c-addr + 2, 4)
    P2ALIGN1 align=tos, scratch=r12
    bl compile_comma

@ Return
    pop {pc}
.endif

@ ------------------------------------------------------------------------------
@ repeat
@ ( orig dest -- )
@ Append the run-time semantics given below to the current definition, resolving
@ the backward reference dest. Resolve the forward reference orig using the
@ location following the appended run-time semantics.
@
@ ( -- )
@ Continue execution at the location given by dest.
@ ------------------------------------------------------------------------------
.if ENABLE_REPEAT == 1
WORD FLAG_COMPILE_IMMEDIATE, "repeat"
    push {lr}

@ Resolve dest
    bl here                             @ ( orig dest -- orig dest orig )
    SWAP                                @ ( orig dest orig -- orig orig dest )
    bl b_comma                          @ ( orig orig dest -- orig )

@ Resolve orig
    bl here                             @ ( orig -- orig dest )
    PUSH_TOS                            @ beq
    ldr tos, =0xF0008000                @ ( orig dest -- orig dest opcode )
    bl bc_comma                         @ ( orig dest opcode -- )

@ Return
    pop {pc}
.endif

@ ------------------------------------------------------------------------------
@ rot
@ ( x1 x2 x3 -- x2 x3 x1 )
@ Rotate the top three stack entries.
@ ------------------------------------------------------------------------------
.if ENABLE_ROT == 1
WORD FLAG_INTERPRET_COMPILE & FOLDS_3, "rot"
    ROT
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ rshift
@ ( x1 u -- x2 )
@ Perform a logical right shift of u bit-places on x1, giving x2. Put zeroes
@ into the most significant bits vacated by the shift. An ambiguous condition
@ exists if u is greater than or equal to the number of bits in a cell.
@ ------------------------------------------------------------------------------
.if ENABLE_RSHIFT == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "rshift"
    ldmia dsp!, {r0}
    lsrs tos, r0, tos
    bx lr
.endif

.if ENABLE_S_TO_D == 1
WORD FLAG_SKIP, "s>d", s_to_d
.endif

.if ENABLE_SIGN == 1
WORD FLAG_SKIP, "sign"
.endif

.if ENABLE_SM_DIV_REM == 1
WORD FLAG_SKIP, "sm/rem", sm_div_rem
.endif

@ ------------------------------------------------------------------------------
@ source
@ ( -- c-addr u )
@ c-addr is the address of, and u is the number of characters in, the input
@ buffer.
@ ------------------------------------------------------------------------------
.if ENABLE_SOURCE == 1
WORD FLAG_INTERPRET_COMPILE, "source"
    ldr r0, =src
    ldmia r0, {r1, r2}
    PUSH_REGS top=r2, from=r1           @ ( -- c-addr u )
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ state
@ ( -- a-addr )
@ a-addr is the address of a cell containing the compilation-state flag. State
@ is true when in compilation state, false otherwise. The true value in state is
@ non-zero, but is otherwise implementation-defined. Only the following standard
@ words alter the value in state: : (colon), ; (semicolon), abort, quit,
@ :noname, [ (left-bracket), ] (right-bracket).
@ ------------------------------------------------------------------------------
.if ENABLE_STATE == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE, "state"
    PUSH_TOS
    ldr tos, =status
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ swap
@ ( x1 x2 -- x2 x1 )
@ Exchange the top two stack items.
@ ------------------------------------------------------------------------------
.if ENABLE_SWAP == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "swap"
    SWAP
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ then
@ ( orig1 | orig2 -- )
@ Append the run-time semantics given below to the current definition. Resolve
@ the forward reference orig1 or orig2 using the location of the appended
@ run-time semantics.
@
@ ( -- )
@ Continue execution.
@ ------------------------------------------------------------------------------
.if ENABLE_THEN == 1
WORD FLAG_COMPILE_IMMEDIATE, "then"
    push {lr}

@ Resolve orig1 or orig2 based on whether LSB is set
    ands r0, tos, #1
    bne 1f
        bl here                         @ ( orig1 -- orig1 dest )
        PUSH_TOS                        @ beq
        ldr tos, =0xF0008000            @ ( orig1 dest -- orig1 dest opcode )
        bl bc_comma                     @ ( orig1 dest opcode -- )
        b 6f                            @ Goto return

@ Resolve orig2
1:  eors tos, r0                        @ Clear bit0
    bl here                             @ ( orig2 -- orig2 dest )
    bl b_comma                          @ ( orig2 dest -- )

@ Return
6:  pop {pc}
.endif

@ ------------------------------------------------------------------------------
@ u<
@ ( u1 u2 -- flag )
@ flag is true if and only if u1 is less than u2.
@ ------------------------------------------------------------------------------
.if ENABLE_U_LESS == 1
WORD FLAG_INTERPRET_COMPILE & FOLDS_2, "u<", u_less
    ldmia dsp!, {r0}
    cmp r0, tos
    ite lo
    movlo tos, #-1
    movhs tos, #0
    bx lr
.endif

.if ENABLE_UM_TIMES == 1
WORD FLAG_SKIP, "um*", um_times
.endif

.if ENABLE_UM_DIV_MOD == 1
WORD FLAG_SKIP, "um/mod", um_div_mod
.endif

@ ------------------------------------------------------------------------------
@ unloop
@ (             -- )
@ ( R: loop-sys -- )
@ Discard the loop-control parameters for the current nesting level. An unloop
@ is required for each nesting level before the definition may be exited. An
@ ambiguous condition exists if the loop-control parameters are unavailable.
@ ------------------------------------------------------------------------------
.if ENABLE_UNLOOP == 1
WORD FLAG_COMPILE_IMMEDIATE, "unloop"
    push {lr}

@ pop {r0, r1}
    PUSH_INT16 #0xBC03                  @ ( -- opcode )
    bl h_comma                          @ ( opcode -- )

@ Return
    pop {pc}
.endif

@ ------------------------------------------------------------------------------
@ until
@ ( dest -- )
@ Append the run-time semantics given below to the current definition, resolving
@ the backward reference dest.
@
@ ( x -- )
@ If all bits of x are zero, continue execution at the location specified by
@ dest.
@ ------------------------------------------------------------------------------
.if ENABLE_UNTIL == 1
WORD FLAG_COMPILE_IMMEDIATE, "until"
    push {lr}

@ Until needs to consume its own flag
@ movs r0, tos
@ ldmia dsp!, {tos}
    PUSH_TOS
    ldr tos, =0xCF400030                @ ( -- opcode )
    bl comma                            @ ( opcode -- )

@ cmp r0, #0
    PUSH_INT16 #0x2800                  @ ( -- opcode )
    bl h_comma                          @ ( opcode -- )

@ Resolve dest
    bl here                             @ ( dest -- dest orig )
    SWAP                                @ ( dest orig -- orig dest )
    PUSH_TOS                            @ beq
    ldr tos, =0xF0008000                @ ( orig dest -- orig dest opcode )
    bl bc_comma                         @ ( orig dest opcode -- )

@ Return
    pop {pc}
.endif

@ ------------------------------------------------------------------------------
@ variable
@ ( source: "<spaces>name" -- )
@ Skip leading space delimiters. Parse name delimited by a space. Create a
@ definition for name with the execution semantics defined below. Reserve one
@ cell of data space at an aligned address. The cell gets zero initialized.
@ name is referred to as a "variable".
@
@ ( -- a-addr )
@ a-addr is the address of the reserved cell.
@ ------------------------------------------------------------------------------
.if ENABLE_VARIABLE == 1
WORD FLAG_INTERPRET_COMPILE, "variable"
    push {lr}

@ Create word
    bl word_comma                       @ ( -- )

@ Write literal with the reserved cells address
    ldr r0, =data_end
    ldr r1, [r0]
    movs r2, #0                         @ Zero initialize cell
    str r2, [r1, #-4]!
    PUSH_REGS r1                        @ ( -- x )
    str r1, [r0]
    bl literal                          @ ( x -- )

@ bx lr
    PUSH_INT16 #0x4770                  @ ( -- opcode )
    bl h_comma                          @ ( opcode -- )

@ Change flags set by create to also reserve a cell
@ r0    link
@ r1    flags
    ldr r0, =link
    ldr r0, [r0]
    movs r1, #FLAG_INTERPRET_COMPILE & RESERVE_1
    strb r1, [r0, #4]

@ Return
    pop {pc}
.endif

@ ------------------------------------------------------------------------------
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
@ ------------------------------------------------------------------------------
.if ENABLE_WHILE == 1
WORD FLAG_COMPILE_IMMEDIATE, "while"
    push {lr}

@ While needs to consume its own flag
@ movs r0, tos
@ ldmia dsp!, {tos}
    PUSH_TOS
    ldr tos, =0xCF400030                @ ( -- opcode )
    bl comma                            @ ( opcode -- )

@ cmp r0, #0
    PUSH_INT16 #0x2800                  @ ( -- opcode )
    bl h_comma                          @ ( opcode -- )

@ Reserve space for orig
    bl here                             @ ( dest -- dest orig )
    SWAP                                @ ( dest orig -- orig dest )
    PUSH_INT8 #4                        @ Reserve space for branch instruction
    bl allot

@ Return
    pop {pc}
.endif

@ ------------------------------------------------------------------------------
@ xor
@ ( x1 x2 -- x3 )
@ x3 is the bit-by-bit exclusive-or of x1 with x2.
@ ------------------------------------------------------------------------------
.if ENABLE_XOR == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "xor"
    ldmia dsp!, {r0}
    eors tos, r0
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ [
@ ( -- )
@ Enter interpretation state. [ is an immediate word.
@ ------------------------------------------------------------------------------
.if ENABLE_BRACKET_LEFT == 1
WORD FLAG_COMPILE_IMMEDIATE, "[", bracket_left
    ldr r0, =status
    movs r1, #0
    str r1, [r0]
    bx lr
.endif

@ ------------------------------------------------------------------------------
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
@ ------------------------------------------------------------------------------
.if ENABLE_BRACKET_TICK == 1
WORD FLAG_COMPILE_IMMEDIATE, "[']", bracket_tick
    push {lr}
    bl tick                             @ ( -- xt )
    bl literal                          @ ( xt -- )
    pop {pc}
.endif

@ ------------------------------------------------------------------------------
@ [
@ ( -- )
@ Enter compilation state.
@ ------------------------------------------------------------------------------
.if ENABLE_BRACKET_RIGHT == 1
WORD FLAG_INTERPRET, "]", bracket_right
    ldr r0, =status
    movs r1, #-1
    str r1, [r0]
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ 0<>
@ ( x -- flag )
@ flag is true if and only if x is not equal to zero.
@ ------------------------------------------------------------------------------
.if ENABLE_ZERO_NE == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "0<>", zero_ne
    cmp tos, #0
    ite ne
    movne tos, #-1                      @ ( x -- flag )
    moveq tos, #0                       @ ( x -- flag )
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ 0>
@ ( n -- flag )
@ flag is true if and only if n is greater than zero.
@ ------------------------------------------------------------------------------
.if ENABLE_ZERO_MORE == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_1, "0>", zero_more
    cmp tos, #0
    ite gt
    movgt tos, #-1                      @ ( n -- flag )
    movle tos, #0                       @ ( n -- flag )
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ 2>r
@ (    x1 x2 --       )
@ ( R:       -- x1 x2 )
@ Transfer cell pair x1 x2 to the return stack. Semantically equivalent to swap
@ >r >r.
@ ------------------------------------------------------------------------------
.if ENABLE_TWO_TO_R == 1
WORD FLAG_COMPILE & FLAG_INLINE, "2>r", two_to_r
    TWO_TO_R
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ 2>r
@ (         -- x1 x2 )
@ (R: x1 x2 --       )
@ Transfer cell pair x1 x2 from the return stack. Semantically equivalent to r>
@ r> swap.
@ ------------------------------------------------------------------------------
.if ENABLE_TWO_R_FROM == 1
WORD FLAG_COMPILE & FLAG_INLINE, "2r>", two_r_from
    TWO_R_FROM
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ 2r@
@ (          -- x1 x2 )
@ ( R: x1 x2 -- x1 x2 )
@ Copy cell pair x1 x2 from the return stack. Semantically equivalent to r> r>
@ 2dup >r >r swap.
@ ------------------------------------------------------------------------------
.if ENABLE_TWO_R_FETCH == 1
WORD FLAG_COMPILE & FLAG_INLINE, "2r@", two_r_fetch
    ldmia sp, {r0, r1}
    PUSH_REGS top=r0, from=r1
    bx lr
.endif

.if ENABLE_COLON_NONAME == 1
WORD FLAG_SKIP, ":noname", colon_noname
.endif

@ ------------------------------------------------------------------------------
@ <>
@ ( x1 x2 -- flag )
@ flag is true if and only if x1 is not bit-for-bit the same as x2.
@ ------------------------------------------------------------------------------
.if ENABLE_NE == 1
WORD FLAG_INTERPRET_COMPILE & FOLDS_2, "<>", ne
    ldmia dsp!, {r0}                    @ ( x1 x2 -- x2 )
    cmp r0, tos
    ite ne
    movne tos, #-1                      @ ( x2 -- flag )
    moveq tos, #0                       @ ( x2 -- flag )
    bx lr
.endif

.if ENABLE_Q_DO == 1
WORD FLAG_SKIP, "?do", q_do
.endif

.if ENABLE_ACTION_OF == 1
WORD FLAG_SKIP, "action-of", action_of
.endif

@ ------------------------------------------------------------------------------
@ again
@ ( dest -- )
@ Append the run-time semantics given below to the current definition, resolving
@ the backward reference dest.
@
@ ( -- )
@ Continue execution at the location specified by dest. If no other control flow
@ words are used, any program code after again will not be executed.
@ ------------------------------------------------------------------------------
.if ENABLE_AGAIN == 1
WORD FLAG_COMPILE_IMMEDIATE, "again"
    push {lr}

@ Resolve branch to begin
    bl here                             @ ( dest -- dest orig )
    SWAP                                @ ( dest orig -- orig dest )
    bl b_comma                          @ ( orig dest -- )

@ Return
    pop {pc}
.endif

.if ENABLE_BUFFER_COLON == 1
WORD FLAG_SKIP, "buffer:", buffer_colon
.endif

@ ------------------------------------------------------------------------------
@ case
@ ( -- )
@ Mark the start of the case...of...endof...endcase structure. Append the
@ run-time semantics given below to the current definition.
@
@ ( -- )
@ Continue execution.
@ ------------------------------------------------------------------------------
.if ENABLE_CASE == 1
WORD FLAG_COMPILE_IMMEDIATE, "case"
@ Increment case_lvl
@ r0    case_lvl address
@ r1    case_lvl
    ldr r0, =case_lvl
    ldrb r1, [r0]
    adds r1, #1
    strb r1, [r0]
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ compile,
@ ( xt -- )
@ Append the execution semantics of the definition represented by xt to the
@ execution semantics of the current definition.
@ ------------------------------------------------------------------------------
.if ENABLE_COMPILE_COMMA == 1
WORD FLAG_COMPILE, "compile,", compile_comma
    push {lr}

@ Data or text
    bl to_text_q                        @ ( -- true | false )
    cmp tos, #0
    beq 1f                              @ Goto compile, data
        b 2f                            @ Goto compile, text

@ compile, data
@ r0    pc-relative address
@ r2    xt
@ tos   data_begin
1:  DROP                                @ ( false -- )
    bl here                             @ ( -- data_begin )
    SWAP                                @ ( xt data_begin -- data_begin xt )
    POP_REGS r2                         @ ( xt -- )
    subs r0, r2, tos                    @ xt - data_begin
    subs r0, #4                         @ pc is 4 bytes ahead in thumb/thumb2!
    b 1f                                @ Goto range check for bl

@ compile, text
@ r0    pc-relative address
@ r2    xt
@ tos   xt
2:  DROP                                @ ( true -- )
    ldr r0, =to_text_begin
    ldmia r0, {r1, r2}
    subs r2, r1                         @ Length of current >text block
    ldr r0, =text_begin
    ldr r0, [r0]
    adds r0, r2                         @ Address current definition would have in text so far
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
@ r0    pc-relative address (xt - (data-space pointer + 4))
@ r1    J1 | J2 | imm11 | imm10
@ tos   opcode
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

    bl rev_comma                        @ ( opcode -- )
        b 6f                            @ Goto return

@ movw movt blx
@ bl coudln't cover our range, do movw movt blx
@ r0    bottom | top
@ r1    intermediate
@ r2    xt + 1
@ tos   opcode
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
    bl rev_comma                        @ ( opcode -- )
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

    bl rev_comma                        @ ( opcode -- )

@ blx r0
    PUSH_INT16 #0x4780                  @ ( -- opcode )
    bl h_comma                          @ ( opcode -- )

@ Return
6:  pop {pc}
.endif

.if ENABLE_DEFER == 1
WORD FLAG_SKIP, "defer"
.endif

.if ENABLE_DEFER_STORE == 1
WORD FLAG_SKIP, "defer!", defer_store
.endif

.if ENABLE_DEFER_FETCH == 1
WORD FLAG_SKIP, "defer@", defer_fetch
.endif

@ ------------------------------------------------------------------------------
@ endcase
@ ( C: case-sys -- )
@ Mark the end of the case...of...endof...endcase structure. Use case-sys to
@ resolve the entire structure. Append the run-time semantics given below to the
@ current definition.
@
@ ( x -- )
@ Discard the case selector x and continue execution
@ ------------------------------------------------------------------------------
.if ENABLE_ENDCASE == 1
WORD FLAG_COMPILE_IMMEDIATE, "endcase"
    push {lr}

@ If the case selector never matched, discard it now before we resolve the
@ branch(es) from endof(s)
@ ldmia dsp!, {tos}
    PUSH_INT16 #0xCF40                  @ ( -- opcode )
    bl h_comma                          @ ( opcode -- )

@ Resolve case-sys and decrement case_lvl
@ r0    case_lvl address
@ r1    case_lvl decremented
@ tos   case_lvl
    PUSH_TOS
    ldr r0, =case_lvl
    ldrb tos, [r0]                      @ ( -- case_lvl )
    subs r1, tos, #1
    strb r1, [r0]
    bl csp_comma                        @ ( case_lvl -- ) ( C: case-sys -- )

@ Return
    pop {pc}
.endif

@ ------------------------------------------------------------------------------
@ endof
@ (    orig --          )
@ ( C:      -- case-sys )
@ Mark the end of the of...endof part of the case structure. The next location
@ for a transfer of control resolves the reference given by orig. Append the
@ run-time semantics given below to the current definition. Resolve the
@ reference given by orig and place case-sys on the control-flow stack, to be
@ resolved by endcase.
@
@ ( -- )
@ Continue execution at the location specified by the consumer of case-sys2.
@ ------------------------------------------------------------------------------
.if ENABLE_ENDOF == 1
WORD FLAG_COMPILE_IMMEDIATE, "endof"
    push {lr}

@ Reverse push case-sys onto the stack
@ r0    csp address
@ r1    csp
@ r2    case_lvl
@ tos   case-sys
    bl here                             @ ( -- case-sys )
    ldr r0, =csp
    ldr r1, [r0]
    ldr r2, =case_lvl
    ldrb r2, [r2]
    strd r2, tos, [r1], #8              @ ( C: -- case-sys )
    str r1, [r0]
    DROP                                @ ( case-sys -- )

@ Reserve space for case-sys
    PUSH_INT8 #4
    bl allot

@ Resolve orig
    bl here                             @ ( orig -- orig dest )
    PUSH_TOS                            @ bne
    ldr tos, =0xF0408000                @ ( orig dest -- orig dest opcode )
    bl bc_comma                         @ ( orig dest opcode -- )

@ Return
    pop {pc}
.endif

.if ENABLE_ERASE == 1
WORD FLAG_SKIP, "erase"
.endif

@ ------------------------------------------------------------------------------
@ false
@ ( -- false )
@ Return a false flag.
@ ------------------------------------------------------------------------------
.if ENABLE_FALSE == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE, "false"
    PUSH_TOS
    movs tos, #0
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ hex
@ ( -- )
@ Set contents of radix to sixteen.
@ ------------------------------------------------------------------------------
.if ENABLE_HEX == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE, "hex"
    ldr r0, =radix
    movs r1, #16
    str r1, [r0]
    bx lr
.endif

.if ENABLE_IS == 1
WORD FLAG_SKIP, "is"
.endif

.if ENABLE_MARKER == 1
WORD FLAG_SKIP, "marker"
.endif

@ ------------------------------------------------------------------------------
@ nip
@ ( x1 x2 -- x2 )
@ Drop the first item below the top of stack.
@ ------------------------------------------------------------------------------
.if ENABLE_NIP == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "nip"
    NIP
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ of
@ ( -- orig )
@ Put orig onto the stack. Append the run-time semantics given below to the
@ current definition. The semantics are incomplete until resolved by a consumer
@ of orig such as endof.
@
@ ( x1 x2 -- | x1 )
@ If the two values on the stack are not equal, discard the top value and
@ continue execution at the location specified by the consumer of of-sys,
@ e.g., following the next endof. Otherwise, discard both values and continue
@ execution in line.
@ ------------------------------------------------------------------------------
.if ENABLE_OF == 1
WORD FLAG_COMPILE_IMMEDIATE, "of"
    push {lr}

@ movs r0, tos
@ ldmia dsp!, {tos}
    PUSH_TOS
    ldr tos, =0xCF400030                @ ( -- opcode )
    bl comma                            @ ( opcode -- )

@ cmp r0, tos
    PUSH_INT16 #0x42B0                  @ ( -- opcode )
    bl h_comma                          @ ( opcode -- )

@ orig
    bl here                             @ ( -- orig )
    PUSH_INT8 #4
    bl allot

@ ldmia dsp!, {tos}
    PUSH_INT16 #0xCF40                  @ ( -- opcode )
    bl h_comma                          @ ( opcode -- )

@ Return
    pop {pc}
.endif

.if ENABLE_PAD == 1
WORD FLAG_SKIP, "pad"
.endif

@ ------------------------------------------------------------------------------
@ parse-name
@ ( "<spaces>name<space>" -- token-addr token-u )
@ Skip leading space delimiters. Parse name delimited by a space.
@
@ token-addr is the address of the selected string within the input buffer and
@ token-u is its length in characters. If the parse area is empty or contains
@ only white space, the resulting string has length zero.
@ ------------------------------------------------------------------------------
.if ENABLE_PARSE_NAME == 1
WORD FLAG_INTERPRET_COMPILE, "parse-name", parse_name
@ r0    c-addr + >in advanced   (= current address)
@ r1    c-addr + u              (= end address)
@ r2    >in
@ r3    character
@ tos   c-addr + >in            (= start address)
    ldr r2, =in                         @ >in
    ldr r2, [r2]
    POP_REGS r1                         @ ( u -- )

    adds r1, tos, r1                    @ c-addr + u
    adds tos, tos, r2                   @ c-addr + >in
    movs r0, tos                        @ Keep start address

@ Skip leading spaces
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

@ Return
6:  bx lr
.endif

@ ------------------------------------------------------------------------------
@ pick
@ ( xu...x1 x0 u -- xu...x1 x0 xu )
@ Remove u. Copy the xu to the top of the stack. An ambiguous condition exists
@ if there are less than u+2 items on the stack before pick is executed.
@ ------------------------------------------------------------------------------
.if ENABLE_PICK == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE, "pick"
    PICK
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ roll
@ ( xu xu-1 ... x0 u -- xu-1 ... x0 xu )
@ Remove u. Rotate u+1 items on the top of the stack. An ambiguous condition
@ exists if there are less than u+2 items on the stack before roll is executed.
@ ------------------------------------------------------------------------------
.if ENABLE_ROLL == 1
WORD FLAG_INTERPRET_COMPILE, "roll"
    lsl r0, tos, #2
    adds r0, dsp, r0
    ldr tos, [r0], #-4
1:  cmp r0, dsp
    blo 1f
        ldr r1, [r0], #-4
        str r1, [r0, #8]
        b 1b
1:  NIP
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ true
@ ( -- true )
@ Return a true flag, a single-cell value with all bits set.
@ ------------------------------------------------------------------------------
.if ENABLE_TRUE == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE, "true"
    PUSH_TOS
    movs tos, #-1
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ tuck
@ ( x1 x2 -- x2 x1 x2 )
@ Copy the first (top) stack item below the second stack item.
@ ------------------------------------------------------------------------------
.if ENABLE_TUCK == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE & FOLDS_2, "tuck"
    TUCK
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ u>
@ ( u1 u2 -- flag )
@ flag is true if and only if u1 is greater than u2.
@ ------------------------------------------------------------------------------
.if ENABLE_U_MORE == 1
WORD FLAG_INTERPRET_COMPILE & FOLDS_2, "u>", u_more
    ldmia dsp!, {r0}                    @ ( u1 u2 -- u2 )
    cmp r0, tos
    ite hi
    movhi tos, #-1                      @ ( u2 -- flag )
    movls tos, #0                       @ ( u2 -- flag )
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ unused
@ ( -- u )
@ u is the amount of space remaining in the region addressed by here, in address
@ units.
@ ------------------------------------------------------------------------------
.if ENABLE_UNUSED == 1
WORD FLAG_INTERPRET, "unused"
    push {lr}
    bl to_text_q                        @ ( -- true | false )
    cmp tos, #0
    ite eq
    ldreq r0, =data_begin
    ldrne r0, =text_begin
    ldmia r0, {r0, tos}
    subs tos, r0
    pop {pc}
.endif

.if ENABLE_VALUE == 1
WORD FLAG_SKIP, "value"
.endif

.if ENABLE_WITHIN == 1
WORD FLAG_SKIP, "within"
.endif

