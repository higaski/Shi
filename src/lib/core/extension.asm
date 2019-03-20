@ Extension word set
@
@ \file   shi.asm
@ \author Vincent Hamp
@ \date   08/06/2017

.section .text

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
@ r1    shi_dict_begin
@ r2    link
@ r3    count links
    ldr r0, =to_text_begin
    ldr r0, [r0]
    cmp r0, #0
    beq 6f                              @ Goto return
    ldr r1, =shi_dict_begin
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
