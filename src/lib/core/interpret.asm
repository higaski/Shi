@ Interpret/compile
@
@ \file   core.asm
@ \author Vincent Hamp
@ \date   27/07/2016

.section .text

@ ------------------------------------------------------------------------------
@ interpret
@ ( -- )
@ ------------------------------------------------------------------------------
.thumb_func
interpret:
    push {lr}

// TODO maybe check for stackoverflow or underflow here?

@ Parse
interpret_parse:
    bl source                           @ ( -- c-addr u )
    bl parse                            @ Parse string
    cmp tos, #0                         @ token-u - 0
    bne interpret_find                  @ Goto find
        TWO_DROP                        @ ( token-addr 0 -- )
        PRINT "'shi' attempt to parse zero-length string >>>interpret<<<"
        b interpret_return              @ Goto return

@ Find
interpret_find:
    SWAP                                @ ( token-addr token-u -- token-u token-addr )
    OVER                                @ ( token-u token-addr -- token-u token-addr token-u )
    bl find                             @ Find token
    cmp tos, #0                         @ flags - 0
    bne interpret_state                 @ Goto state check

@ Number
interpret_number:
    DROP                                @ ( false -- )
    SWAP                                @ ( token-u token-addr -- token-addr token-u )
    bl number                           @ ( -- n true | false ) try numbers if we didn't find anything
    POP_REGS r0                         @ ( flag -- )
    cmp r0, #0                          @ flag - 0
    bne interpret_set_lfp               @ Goto set literal-folding pointer
    PRINT "'shi' undefined word >>>interpret<<<"
        b interpret_return              @ Goto return

@ Set literal-folding pointer
interpret_set_lfp:
    cmp lfp, #0                         @ Save stackpointer in case literal-folding pointer is still zero
    it eq
    movseq lfp, dsp
    b interpret_done                    @ Goto done

@ State check
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

@ Interpret
interpret_execute:
    ands tos, #~FLAG_INTERPRET
    beq 1f                              @ Goto execute
        PRINT "'shi' interpreting a compile-only word >>>interpret<<<"
        b interpret_return              @ Goto return

@ Execute
1:  movs lfp, #0                        @ Clear literal-folding pointer
    DROP                                @ ( flags -- )
    bl execute                          @ Execute xt
        b interpret_done                @ Goto done

@ Compile
interpret_compile:
    ands r0, tos, #~FLAG_COMPILE
    beq 1f                              @ Goto continue
        PRINT "'shi' compiling an interpret-only word >>>interpret<<<"
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
@ r12   folds bits
    subs r2, dsp, #4                    @ Get number of literals on stack
    subs r3, lfp, r2
    lsrs r3, #2
    mvns r12, r1                        @ Invert flags
    ands r12, r12, #BIT_FOLDS           @ Extract folds bits from flags
    beq 1f                              @ Goto literal
        cmp r12, r3                     @ Folds bits - number of literals
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

@ compile, or inline,?
@ r0    xt
@ r1    flags
4:  PUSH_REGS r0                        @ ( -- xt )
    ands r1, #~FLAG_INLINE
    bne 5f
        bl inline_comma
        b interpret_done                @ Goto done
5:  bl compile_comma

@ Done
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

@ Return
interpret_return:
    SET_SOURCE #0, #0                   @ Clear source
    pop {pc}
.ltorg
