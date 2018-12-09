.section .text

/***************************************************************************//**
@ inline,
@ ( xt -- )
@ Inlines code from xt in the memory-space.
 ******************************************************************************/
.thumb_func
inline_comma:
    push {lr}

@ Copy opcodes from xt
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

@ Return
6:  pop {pc}
