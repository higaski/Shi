@ Extension word set
@
@ \file   extension.asm
@ \author Vincent Hamp
@ \date   08/06/2017

.section .text

//.extern tick_ms;

WORD FLAG_INTERPRET_COMPILE, "extension_test"
    //ldr r0, =tick_ms
    //ldr r0, [r0]
    nop
    nop
    bx lr
