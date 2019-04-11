@ Api to C/C++
@
@ \file   api.asm
@ \author Vincent Hamp
@ \date   27/07/2016

.syntax unified
.thumb
.arch armv7-m

.global shi_init_asm, shi_evaluate_asm, shi_variable_asm, shi_clear_asm, shi_tick_asm

.section .text

@ ------------------------------------------------------------------------------
@ Register mapping
@ ------------------------------------------------------------------------------
@ Map top of stack and stackpointer to last low registers and literal-folding
@ pointer to first high register (hardwired, can not be changed)
tos .req r6                             @ Top of stack
dsp .req r7                             @ Data-stack pointer
lfp .req r8                             @ Literal-folding pointer

@ ------------------------------------------------------------------------------
@ Common
@ ------------------------------------------------------------------------------
#include "shi.hpp"

@ ------------------------------------------------------------------------------
@ C -> assembler
@ ------------------------------------------------------------------------------
.equ STACK_SIZE, SHI_STACK_SIZE
.equ ERASED_WORD, SHI_ERASED_WORD
.equ ENABLE_NUMBER_PREFIX, SHI_ENABLE_NUMBER_PREFIX
.equ ENABLE_PRINT, SHI_ENABLE_PRINT
.equ ENABLE_REDEFINITION, SHI_ENABLE_REDEFINITION

@ Core words
.equ ENABLE_STORE, SHI_ENABLE_STORE
.equ ENABLE_TICK, SHI_ENABLE_TICK
.equ ENABLE_TIMES, SHI_ENABLE_TIMES
.equ ENABLE_TIMES_DIV, SHI_ENABLE_TIMES_DIV
.equ ENABLE_TIMES_DIV_MOD, SHI_ENABLE_TIMES_DIV_MOD
.equ ENABLE_PLUS, SHI_ENABLE_PLUS
.equ ENABLE_PLUS_STORE, SHI_ENABLE_PLUS_STORE
.equ ENABLE_PLUS_LOOP, SHI_ENABLE_PLUS_LOOP
.equ ENABLE_COMMA, SHI_ENABLE_COMMA
.equ ENABLE_MINUS, SHI_ENABLE_MINUS
.equ ENABLE_DIV, SHI_ENABLE_DIV
.equ ENABLE_DIV_MOD, SHI_ENABLE_DIV_MOD
.equ ENABLE_ZERO_LESS, SHI_ENABLE_ZERO_LESS
.equ ENABLE_ZERO_EQUAL, SHI_ENABLE_ZERO_EQUAL
.equ ENABLE_ONE_PLUS, SHI_ENABLE_ONE_PLUS
.equ ENABLE_ONE_MINUS, SHI_ENABLE_ONE_MINUS
.equ ENABLE_TWO_STORE, SHI_ENABLE_TWO_STORE
.equ ENABLE_TWO_TIMES, SHI_ENABLE_TWO_TIMES
.equ ENABLE_TWO_DIV, SHI_ENABLE_TWO_DIV
.equ ENABLE_TWO_FETCH, SHI_ENABLE_TWO_FETCH
.equ ENABLE_TWO_DROP, SHI_ENABLE_TWO_DROP
.equ ENABLE_TWO_DUP, SHI_ENABLE_TWO_DUP
.equ ENABLE_TWO_OVER, SHI_ENABLE_TWO_OVER
.equ ENABLE_TWO_SWAP, SHI_ENABLE_TWO_SWAP
.equ ENABLE_COLON, SHI_ENABLE_COLON
.equ ENABLE_SEMI, SHI_ENABLE_SEMI
.equ ENABLE_LESS, SHI_ENABLE_LESS
.equ ENABLE_EQUAL, SHI_ENABLE_EQUAL
.equ ENABLE_MORE, SHI_ENABLE_MORE
.equ ENABLE_TO_BODY, SHI_ENABLE_TO_BODY
.equ ENABLE_TO_IN, SHI_ENABLE_TO_IN
.equ ENABLE_TO_R, SHI_ENABLE_TO_R
.equ ENABLE_Q_DUP, SHI_ENABLE_Q_DUP
.equ ENABLE_FETCH, SHI_ENABLE_FETCH
.equ ENABLE_ABS, SHI_ENABLE_ABS
.equ ENABLE_ALIGN, SHI_ENABLE_ALIGN
.equ ENABLE_ALIGNED, SHI_ENABLE_ALIGNED
.equ ENABLE_ALLOT, SHI_ENABLE_ALLOT
.equ ENABLE_AND, SHI_ENABLE_AND
.equ ENABLE_BASE, SHI_ENABLE_BASE
.equ ENABLE_BEGIN, SHI_ENABLE_BEGIN
.equ ENABLE_C_STORE, SHI_ENABLE_C_STORE
.equ ENABLE_C_COMMA, SHI_ENABLE_C_COMMA
.equ ENABLE_C_FETCH, SHI_ENABLE_C_FETCH
.equ ENABLE_CELL_PLUS, SHI_ENABLE_CELL_PLUS
.equ ENABLE_CELLS, SHI_ENABLE_CELLS
.equ ENABLE_CHAR_PLUS, SHI_ENABLE_CHAR_PLUS
.equ ENABLE_CHARS, SHI_ENABLE_CHARS
.equ ENABLE_CONSTANT, SHI_ENABLE_CONSTANT
.equ ENABLE_CREATE, SHI_ENABLE_CREATE
.equ ENABLE_DECIMAL, SHI_ENABLE_DECIMAL
.equ ENABLE_DEPTH, SHI_ENABLE_DEPTH
.equ ENABLE_DO, SHI_ENABLE_DO
.equ ENABLE_DOES, SHI_ENABLE_DOES
.equ ENABLE_DROP, SHI_ENABLE_DROP
.equ ENABLE_DUP, SHI_ENABLE_DUP
.equ ENABLE_ELSE, SHI_ENABLE_ELSE
.equ ENABLE_EXECUTE, SHI_ENABLE_EXECUTE
.equ ENABLE_EXIT, SHI_ENABLE_EXIT
.equ ENABLE_FILL, SHI_ENABLE_FILL
.equ ENABLE_FIND, SHI_ENABLE_FIND
.equ ENABLE_FM_DIV_MOD, SHI_ENABLE_FM_DIV_MOD
.equ ENABLE_HERE, SHI_ENABLE_HERE
.equ ENABLE_I, SHI_ENABLE_I
.equ ENABLE_IF, SHI_ENABLE_IF
.equ ENABLE_IMMEDIATE, SHI_ENABLE_IMMEDIATE
.equ ENABLE_INVERT, SHI_ENABLE_INVERT
.equ ENABLE_J, SHI_ENABLE_J
.equ ENABLE_LEAVE, SHI_ENABLE_LEAVE
.equ ENABLE_LITERAL, SHI_ENABLE_LITERAL
.equ ENABLE_LOOP, SHI_ENABLE_LOOP
.equ ENABLE_LSHIFT, SHI_ENABLE_LSHIFT
.equ ENABLE_M_TIMES, SHI_ENABLE_M_TIMES
.equ ENABLE_MAX, SHI_ENABLE_MAX
.equ ENABLE_MIN, SHI_ENABLE_MIN
.equ ENABLE_MOD, SHI_ENABLE_MOD
.equ ENABLE_MOVE, SHI_ENABLE_MOVE
.equ ENABLE_NEGATE, SHI_ENABLE_NEGATE
.equ ENABLE_OR, SHI_ENABLE_OR
.equ ENABLE_OVER, SHI_ENABLE_OVER
.equ ENABLE_POSTPONE, SHI_ENABLE_POSTPONE
.equ ENABLE_R_FROM, SHI_ENABLE_R_FROM
.equ ENABLE_R_FETCH, SHI_ENABLE_R_FETCH
.equ ENABLE_RECURSE, SHI_ENABLE_RECURSE
.equ ENABLE_REPEAT, SHI_ENABLE_REPEAT
.equ ENABLE_ROT, SHI_ENABLE_ROT
.equ ENABLE_RSHIFT, SHI_ENABLE_RSHIFT
.equ ENABLE_S_TO_D, SHI_ENABLE_S_TO_D
.equ ENABLE_SIGN, SHI_ENABLE_SIGN
.equ ENABLE_SM_DIV_REM, SHI_ENABLE_SM_DIV_REM
.equ ENABLE_SOURCE, SHI_ENABLE_SOURCE
.equ ENABLE_STATE, SHI_ENABLE_STATE
.equ ENABLE_SWAP, SHI_ENABLE_SWAP
.equ ENABLE_THEN, SHI_ENABLE_THEN
.equ ENABLE_U_LESS, SHI_ENABLE_U_LESS
.equ ENABLE_UM_TIMES, SHI_ENABLE_UM_TIMES
.equ ENABLE_UM_DIV_MOD, SHI_ENABLE_UM_DIV_MOD
.equ ENABLE_UNLOOP, SHI_ENABLE_UNLOOP
.equ ENABLE_UNTIL, SHI_ENABLE_UNTIL
.equ ENABLE_VARIABLE, SHI_ENABLE_VARIABLE
.equ ENABLE_WHILE, SHI_ENABLE_WHILE
.equ ENABLE_XOR, SHI_ENABLE_XOR
.equ ENABLE_BRACKET_LEFT, SHI_ENABLE_BRACKET_LEFT
.equ ENABLE_BRACKET_TICK, SHI_ENABLE_BRACKET_TICK
.equ ENABLE_BRACKET_RIGHT, SHI_ENABLE_BRACKET_RIGHT
.equ ENABLE_ZERO_NE, SHI_ENABLE_ZERO_NE
.equ ENABLE_ZERO_MORE, SHI_ENABLE_ZERO_MORE
.equ ENABLE_TWO_TO_R, SHI_ENABLE_TWO_TO_R
.equ ENABLE_TWO_R_FROM, SHI_ENABLE_TWO_R_FROM
.equ ENABLE_TWO_R_FETCH, SHI_ENABLE_TWO_R_FETCH
.equ ENABLE_NE, SHI_ENABLE_NE
.equ ENABLE_Q_DO, SHI_ENABLE_Q_DO
.equ ENABLE_ACTION_OF, SHI_ENABLE_ACTION_OF
.equ ENABLE_AGAIN, SHI_ENABLE_AGAIN
.equ ENABLE_BUFFER_COLON, SHI_ENABLE_BUFFER_COLON
.equ ENABLE_CASE, SHI_ENABLE_CASE
.equ ENABLE_COMPILE_COMMA, SHI_ENABLE_COMPILE_COMMA
.equ ENABLE_ENDCASE, SHI_ENABLE_ENDCASE
.equ ENABLE_ENDOF, SHI_ENABLE_ENDOF
.equ ENABLE_ERASE, SHI_ENABLE_ERASE
.equ ENABLE_FALSE, SHI_ENABLE_FALSE
.equ ENABLE_HEX, SHI_ENABLE_HEX
.equ ENABLE_NIP, SHI_ENABLE_NIP
.equ ENABLE_OF, SHI_ENABLE_OF
.equ ENABLE_PARSE_NAME, SHI_ENABLE_PARSE_NAME
.equ ENABLE_PICK, SHI_ENABLE_PICK
.equ ENABLE_ROLL, SHI_ENABLE_ROLL
.equ ENABLE_TRUE, SHI_ENABLE_TRUE
.equ ENABLE_TUCK, SHI_ENABLE_TUCK
.equ ENABLE_U_MORE, SHI_ENABLE_U_MORE
.equ ENABLE_UNUSED, SHI_ENABLE_UNUSED
.equ ENABLE_VALUE, SHI_ENABLE_VALUE
.equ ENABLE_WITHIN, SHI_ENABLE_WITHIN

@ Extension words
.equ ENABLE_H_STORE, SHI_ENABLE_H_STORE
.equ ENABLE_H_FETCH, SHI_ENABLE_H_FETCH
.equ ENABLE_TO_TEXT_Q, SHI_ENABLE_TO_TEXT_Q
.equ ENABLE_TO_DATA_Q, SHI_ENABLE_TO_DATA_Q
.equ ENABLE_TO_TEXT, SHI_ENABLE_TO_TEXT
.equ ENABLE_TO_DATA, SHI_ENABLE_TO_DATA

.include "extern.asm"
.include "data.asm"
.include "macros.asm"
.include "comma.asm"
.include "interpret.asm"
.include "number.asm"

@ ------------------------------------------------------------------------------
@ Dictionary
@ ------------------------------------------------------------------------------
shi_dict_begin:                         @ Start of dictionaty

.include "core.asm"
.include "extension.asm"

.section .data

shi_dict_end:                           @ End of dictionary
WORD_TAIL FLAG_SKIP

.section .text

@ ------------------------------------------------------------------------------
@ Initialize shi
@ r0    init_t address
@ ------------------------------------------------------------------------------
.thumb_func
shi_init_asm:
    push {tos-lfp, lr}                  @ ( R: -- tos dsp lfp lr )

@ Store data addresses
@ r0    init_t address
@ r1    data_begin
@ r2    data_end
@ r3    data_begin address
    ldmia r0!, {r1, r2}
    ldr r3, =data_begin
    stmia r3!, {r1, r2}

@ Store text addresses
@ r0    init_t address
@ r1    text_begin
@ r2    text_end
@ r3    text_begin address
    ldmia r0!, {r1, r2}
    stmia r3, {r1, r2}
    cmp r1, r2
    itt ne
    ldrne r3, =shi_dict_end             @ Set last link of core to user dictionary
    strne r1, [r3]

@ Store text alignment
@ r0    init_t address
@ r1    text_align
@ r2    text_align address
    ldrb r0, [r0]
    movs r1, #1
    lsl r1, r0
    ldr r2, =text_align
    strb r1, [r2]

@ Reserve data and set text_begin to last text link
    bl sweep_text

@ Fill data
    bl fill_data

@ Set context (tos dsp and lfp)
    movs lfp, #0                        @ Put zero into lfp...
    movs tos, #'*'                      @ Put stars onto tos ;)
    ldr dsp, =shi_stack_end             @ Reset data-stack pointer

@ Return
    EXIT                                @ Store context
    pop {tos-lfp, pc}                   @ ( R: tos dsp lfp lr -- )

@ ------------------------------------------------------------------------------
@ Search through text dictionary to look for definitions which need to reserve
@ data. Afterwards set text_begin to last found link in text.
@ ------------------------------------------------------------------------------
.thumb_func
sweep_text:
@ r0    link n
@ r1    link n+1
@ r2    flags
@ r3    data_end address
@ r12   data_end
    ldr r1, =shi_dict_begin
    ldr r3, =data_end
    ldr r12, [r3]
1:  ldrb r2, [r1, #4]                   @ flags
    mvns r2, r2                         @ Invert flags
    ands r2, r2, #BIT_RESERVE_RAM       @ Extract reserve ram bits from flags
    subs r12, r2                        @ If bits are set subtract amount of bytes from data_end
    ldr r1, [r1]                        @ link
    cmp r1, #LINK_INVALID               @ End of dictionary?
    itt ne
    movne r0, r1
    bne 1b                              @ Goto search
        str r12, [r3]                   @ Store data_end
        ldr r1, =text_begin
        str r0, [r1]                    @ text_begin is at last found link

@ Return
    bx lr

@ ------------------------------------------------------------------------------
@ Fill data with 0xFF
@ ------------------------------------------------------------------------------
.thumb_func
fill_data:
@ r0    data_begin
@ r1    data_end
@ r2    erased word
    ldr r2, =data_begin
    ldmia r2, {r0, r1}
    movs r2, #ERASED_WORD
1:  cmp r0, r1
    beq 6f
        str r2, [r0], #4                @TODO write last few bytes single... who says data is 4-byte aligned?
        b 1b

@ Return
6:  bx lr

@ ------------------------------------------------------------------------------
@ Forth evaluate
@ r0    c-addr  (cstring address)
@ r1    u       (cstring length)
@ ------------------------------------------------------------------------------
.thumb_func
shi_evaluate_asm:
    push {tos-lfp, lr}                  @ ( R: -- tos dsp lfp lr )

@ Enter forth
1:  ENTRY                               @ Restore context

@ Store source
@ r0    c-addr
@ r1    u
@ r2    src address
    ldr r2, =src
    strd r0, r1, [r2]

@ Set >IN 0
    SET_IN #0                           @ Set >in zero

@ Call interpret
    bl interpret

@ Leave forth
    EXIT                                @ Store context

@ Return
6:  pop {tos-lfp, pc}                   @ ( R: tos dsp lfp lr -- )

@ ------------------------------------------------------------------------------
@ Forth variable
@ r0    c-addr  (cstring address)
@ r1    u       (cstring length)
@ ------------------------------------------------------------------------------
.thumb_func
shi_variable_asm:
    push {tos-lfp, lr}                  @ ( R: -- tos dsp lfp lr )

@ Enter forth
    ENTRY                               @ Restore context

@ Store source
@ r0    c-addr
@ r1    u
@ r2    src address
    ldr r2, =src
    strd r0, r1, [r2]

@ Set >IN 0
    SET_IN #0                           @ Set >in zero

@ Create word
    bl word_comma

@ Write literal with the C variables address
    bl literal

@ bx lr
    PUSH_INT16 #0x4770
    bl h_comma

@ Leave forth
    EXIT                                @ Store context

@ Return
    pop {tos-lfp, pc}                   @ ( R: tos dsp lfp lr -- )

@ ------------------------------------------------------------------------------
@ Clear stack
@ ------------------------------------------------------------------------------
.thumb_func
shi_clear_asm:
    push {tos-lfp, lr}                  @ ( R: -- tos dsp lfp lr )

@ Clear stack from begin to end
@ r0    shi_stack_begin
@ tos   0
@ dsp   shi_stack_end
    movs tos, #0
    ldr dsp, =shi_stack_end
    ldr r0, =shi_stack_begin
1:  cmp r0, dsp
    beq 6f                              @ Goto return
        str tos, [r0], #4
        b 1b

@ Return
6:  movs lfp, #0                        @ Put zero into lfp...
    movs tos, #'*'                      @ Put stars onto tos ;)
    EXIT                                @ Store context
    pop {tos-lfp, pc}                   @ ( R: tos dsp lfp lr -- )
.ltorg

@ ------------------------------------------------------------------------------
@ Tick
@ r0    c-addr  (cstring address)
@ r1    u       (cstring length)
@ ------------------------------------------------------------------------------
.thumb_func
shi_tick_asm:
    push {tos-lfp, lr}                  @ ( R: -- tos dsp lfp lr )

@ Enter forth
1:  ENTRY                               @ Restore context

@ Find
    PUSH_REGS top=r1, from=r0           @ ( -- c-addr u )
    bl find                             @ ( -- token-addr false | xt flags )
    ldr r0, [dsp]
    orrs r0, #1                         @ xt + 1
    TWO_DROP                            @ ( -- )

@ Leave forth
    EXIT                                @ Store context

@ Return
    pop {tos-lfp, pc}                   @ ( R: tos dsp lfp lr -- )
