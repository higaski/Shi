@ Extern dependencies
@
@ \file   extern.asm
@ \author Vincent Hamp
@ \date   03/12/2018

.extern shi_printf, shi_write_flash
.weak shi_printf, shi_write_flash

/***************************************************************************//**
@ shi_printf stub
 ******************************************************************************/
.ifdef PRINT_ENABLED
.thumb_func
shi_printf:
    bx lr
.endif

/***************************************************************************//**
@ shi_write_flash stub
 ******************************************************************************/
.thumb_func
shi_write_flash:
    movs r0, r2
    bx lr
