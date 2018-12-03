@ Extension word set
@
@ \file   extension.asm
@ \author Vincent Hamp
@ \date   08/06/2017

.section .text

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

@ Resolve branch to begin
    bl here                             @ ( -- orig )
    SWAP
    bl b_comma

@ Return
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

@ If the case selector never matched, discard it now
@ before we resolve the branch(es) from endof(s)
@ ldmia dsp!, {tos}
    PUSH_INT16 #0xCF40
    bl h_comma

@ Resolve branch(es) from endof(s)
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
        PRINT "'shi' stack overflow >>>endcase<<<"
        b 6f

@ Take care of endof(s)
@ r0    csp address
@ r1    csp
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

@ Return
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

@ Reverse push orig onto the stack
@ r0    csp address
@ r1    csp
    bl here                             @ ( -- orig )
    ldr r0, =csp
    ldr r1, [r0]
    stmia r1!, {tos}
    str r1, [r0]
    DROP                                @ ( orig -- )

@ Reserve space for orig
    PUSH_INT8 #4
    bl allot

@ Resolve branch from of
    bl here                             @ ( -- dest )
    bl bne_comma

@ Return
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

@ of-sys
    bl here                             @ ( -- of-sys )
    PUSH_INT8 #4
    bl allot

@ ldmia dsp!, {tos}
    PUSH_INT16 #0xCF40
    bl h_comma

@ Return
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

/***************************************************************************//**
@ roll
@ ( xu xu-1 ... x0 u -- xu-1 ... x0 xu )
@ Remove u. Rotate u+1 items on the top of the stack. An ambiguous condition
@ exists if there are less than u+2 items on the stack before roll is executed.
 ******************************************************************************/
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
WORD FLAG_INTERPRET, "unused"
    push {lr}
    bl comma_q                          @ ( -- true | false )
    cmp tos, #0
    ite eq
    ldreq r0, =ram_begin
    ldrne r0, =flash_begin
    ldmia r0, {r0, tos}
    subs tos, r0
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
