@ Extern dependencies
@
@ \file   extern.asm
@ \author Vincent Hamp
@ \date   03/12/2018

.extern shi_printf, shi_write_text
.weak shi_printf, shi_write_text

@ ------------------------------------------------------------------------------
@ shi_printf stub
@ ------------------------------------------------------------------------------
.ifdef PRINT_ENABLED
.thumb_func
shi_printf:
    bx lr
.endif

@ ------------------------------------------------------------------------------
@ shi_write_text stub
@ ------------------------------------------------------------------------------
.thumb_func
shi_write_text:
    movs r0, r2
    bx lr
