@ Extension word set
@
@ \file   extension.asm
@ \author Vincent Hamp
@ \date   08/06/2017

.section .text

.if ENABLE_DOT_COMMENT == 1
WORD FLAG_SKIP, ".(", dot_comment
.endif

.if ENABLE_DOT_R == 1
WORD FLAG_SKIP, ".r", dot_r
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
    movne tos, #-1
    moveq tos, #0
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
    movgt tos, #-1
    movle tos, #0
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
    ldmia dsp!, {r0}
    cmp r0, tos
    ite ne
    movne tos, #-1
    moveq tos, #0
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
    bl here                             @ ( -- orig )
    SWAP
    bl b_comma

@ Return
    pop {pc}
.endif

.if ENABLE_BUFFER_COLON == 1
WORD FLAG_SKIP, "buffer:", buffer_colon
.endif

.if ENABLE_C_Q == 1
WORD FLAG_SKIP, "c\"", c_q
.endif

@ ------------------------------------------------------------------------------
@ case
@ ( -- case-sys  \ technically there ain't nothing to "mark" )
@ Mark the start of the case...of...endof...endcase structure. Append the
@ run-time semantics given below to the current definition.
@
@ ( -- )
@ Continue execution.
@ ------------------------------------------------------------------------------
.if ENABLE_CASE == 1
WORD FLAG_COMPILE_IMMEDIATE, "case"
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

    bl rev_comma                        @ Write opcode
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
@ ( case-sys -- )
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
    PUSH_INT16 #0xCF40
    bl h_comma

@ Take care of endof(s)
    bl csp_comma

@ Return
    pop {pc}
.endif

@ ------------------------------------------------------------------------------
@ endof
@ ( C: case-sys1 of-sys -- case-sys2 )
@ Mark the end of the of...endof part of the case structure. The next location
@ for a transfer of control resolves the reference given by of-sys. Append the
@ run-time semantics given below to the current definition. Replace case-sys1
@ with case-sys2 on the control-flow stack, to be resolved by endcase.
@
@ ( -- )
@ Continue execution at the location specified by the consumer of case-sys2.
@ ------------------------------------------------------------------------------
.if ENABLE_ENDOF == 1
WORD FLAG_COMPILE_IMMEDIATE, "endof"
    push {lr}

@ Reverse push case-sys2 onto the stack
@ r0    csp address
@ r1    csp
    bl here
    ldr r0, =csp
    ldr r1, [r0]
    stmia r1!, {tos}
    str r1, [r0]
    DROP

@ Reserve space for orig
    PUSH_INT8 #4
    bl allot

@ Resolve of-sys
    bl here
    bl bne_comma

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

.if ENABLE_HOLDS == 1
WORD FLAG_SKIP, "holds"
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
@ ------------------------------------------------------------------------------
.if ENABLE_OF == 1
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

@ of-sys
    bl here
    PUSH_INT8 #4
    bl allot

@ ldmia dsp!, {tos}
    PUSH_INT16 #0xCF40
    bl h_comma

@ Return
    pop {pc}
.endif

.if ENABLE_PAD == 1
WORD FLAG_SKIP, "pad"
.endif

@ ------------------------------------------------------------------------------
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
@ ------------------------------------------------------------------------------
.if ENABLE_PARSE == 1
WORD FLAG_INTERPRET_COMPILE, "parse"
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

.if ENABLE_PARSE_NAME == 1
WORD FLAG_SKIP, "parse-name", parse_name
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

.if ENABLE_REFILL == 1
WORD FLAG_SKIP, "refill"
.endif

.if ENABLE_RESTORE_INPUT == 1
WORD FLAG_SKIP, "restore-input", restore_input
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

.if ENABLE_S_EQ == 1
WORD FLAG_SKIP, "s\"", s_eq
.endif

.if ENABLE_SAVE_INPUT == 1
WORD FLAG_SKIP, "save-input", save_input
.endif

.if ENABLE_SOURCE_ID == 1
WORD FLAG_SKIP, "source-id", source_id
.endif

.if ENABLE_TO == 1
WORD FLAG_SKIP, "to"
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

.if ENABLE_U_DOT_R == 1
WORD FLAG_SKIP, "u.r", u_dot_r
.endif

@ ------------------------------------------------------------------------------
@ u>
@ ( u1 u2 -- flag )
@ flag is true if and only if u1 is greater than u2.
@ ------------------------------------------------------------------------------
.if ENABLE_U_MORE == 1
WORD FLAG_INTERPRET_COMPILE & FOLDS_2, "u>", u_more
    ldmia dsp!, {r0}
    cmp r0, tos
    ite hi
    movhi tos, #-1
    movls tos, #0
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

.if ENABLE_BRACKET_COMPILE == 1
WORD FLAG_SKIP, "[compile]", bracket_compile
.endif

.if ENABLE_BS == 1
WORD FLAG_SKIP, "\\", bs
.endif


// NON-ANS (AKA MY OWN) EXTENSIONS ->

@ ------------------------------------------------------------------------------
@ binary
@ ( -- )
@ Set contents of radix to two.
@ ------------------------------------------------------------------------------
.if ENABLE_BINARY == 1
WORD FLAG_INTERPRET_COMPILE & FLAG_INLINE, "binary"
    ldr r0, =radix
    movs r1, #2
    str r1, [r0]
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ c-variable
@ ( source: "<spaces>name" -- )
@ (                 a-addr -- )
@ Skip leading space delimiters. Parse name delimited by a space. Create a
@ definition for name with the execution semantics defined below.
@
@ ( -- a-addr )
@ a-addr is the address of the referenced C variable
@ ------------------------------------------------------------------------------
.if ENABLE_C_VARIABLE == 1
WORD FLAG_INTERPRET_COMPILE, "c-variable", c_variable
    push {lr}

@ Create word
    bl word_comma

@ Write literal with the C variables address
    bl literal

@ bx lr
    PUSH_INT16 #0x4770
    bl h_comma

@ Return
    pop {pc}
.endif

@ ------------------------------------------------------------------------------
@ >text?
@ ( -- true | false )
@ Return true if compiler is currently compiling to text. Return false if
@ compiler is currently compiling to data.
@ ------------------------------------------------------------------------------
.if ENABLE_TO_TEXT_Q == 1
WORD FLAG_INTERPRET, ">text?", to_text_q
    PUSH_TOS
    ldr r0, =to_text_begin
    ldr r0, [r0]
    cmp r0, #0
    ite ne
    movne tos, #-1
    moveq tos, #0
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ >data?
@ ( -- true | false )
@ Return true if compiler is currently compiling to data. Return false if
@ compiler is currently compiling to text.
@ ------------------------------------------------------------------------------
.if ENABLE_TO_DATA_Q == 1
WORD FLAG_INTERPRET, ">data?", to_data_q
    push {lr}
    bl to_text_q
    mvn tos, tos
    pop {pc}
.endif

@ ------------------------------------------------------------------------------
@ >text
@ ( -- )
@
@ ------------------------------------------------------------------------------
.if ENABLE_TO_TEXT == 1
WORD FLAG_INTERPRET, ">text", to_text
    ldr r0, =to_text_begin
    ldr r1, [r0]
    cmp r1, #0
    bne 1f
    ldr r1, =data_begin
    ldr r1, [r1]
    str r1, [r0]
1:  bx lr
.endif

@ ------------------------------------------------------------------------------
@ >data
@ ( -- )
@
@ ------------------------------------------------------------------------------
.if ENABLE_TO_DATA == 1
WORD FLAG_INTERPRET, ">data", to_data
    push {lr}

@ Push data links to return stack
@ r0    to_text_begin
@ r1    s_shi_dict
@ r2    link
@ r3    count links
    ldr r0, =to_text_begin
    ldr r0, [r0]
    cmp r0, #0
    beq 6f                              @ Goto return
    ldr r1, =s_shi_dict
    ldr r2, =link
    movs r3, #0
1:  ldr r2, [r2]
    cmp r2, r0
    blo 1f
        cmp r2, r1
        beq 1f
            push {r2}
            adds r3, #1
            b 1b

@ Return if no links were found
1:  cmp r3, #0
    beq 6f                              @ Goto return

@ Update link with last link before >text
    ldr r0, =link
    str r2, [r0]

@ Pop data links from return stack and calculate equivalent text links to
@ replace them. Also overwrite to_text_begin if it deviates from the very first
@ link. This is important to make sure that the first thing written to text is a
@ link and nothing else!
@ r0    text_begin
@ r1    link n
@ r2    link n+1
@ r3    count links
    pop {r1}
    ldr r0, =to_text_begin
    str r1, [r0]
    ldr r0, =text_begin
    ldr r0, [r0]
1:  cmp r3, #1
    bls 1f
        pop {r2}
        subs r2, r1
        adds r0, r2
        str r0, [r1]
        adds r1, r2
        subs r3, #1
        b 1b

@ Last text link needs to be calculated from current data_begin
@ r0    text_begin
@ r1    link n
@ r2    link n+1
1:  ldr r2, =data_begin
    ldr r2, [r2]
    subs r2, r1
    adds r0, r2

@ Align text_begin before writing last text link
@ r0    text_begin
@ r1    link n
@ r2    text_align
@ r3    text_align - 1
@ r12   unaligned bits
    ldr r2, =text_align
    ldrb r2, [r2]
    subs r3, r2, #1
    ands r12, r0, r3                    @ "Unaligned" bits
    itt ne
    subne r2, r12                       @ Alignment - "unaligned" bits
    addne r0, r2
    str r0, [r1]                        @ Write last text link
    movs r12, r0

@ Store new text_begin and call shi_write_text
@ r0    to_text_begin
@ r1    data_begin
@ r2    text_begin
@ r3    text_begin address
@ r12   text_begin after write
    ldr r2, =to_text_begin
    ldmia r2, {r0, r1}
    ldr r3, =text_begin
    ldr r2, [r3]
    str r12, [r3]
    bl shi_write_text

@ Clear >text block from data
@ r0    to_text_begin
@ r1    data_begin
@ r2    erased word
    ldr r0, =to_text_begin
    ldr r0, [r0]
    ldr r2, =data_begin
    ldr r1, [r2]
    str r0, [r2]                        @ Store data_begin as before >text
    movs r2, #ERASED_WORD
1:  cmp r1, r0
    bls 6f
        strh r2, [r1, #-2]!
        b 1b

@ Return
6:  ldr r0, =to_text_begin
    movs r1, #0
    str r1, [r0]
    pop {pc}
.endif
