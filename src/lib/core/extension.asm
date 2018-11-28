/***************************************************************************//**
 *  @brief      Extension word set
 *
 *  @file       extension.asm
 *  @version    0.1
 *  @author     Vincent Hamp
 *  @date       08/06/2017
 ******************************************************************************/

//.extern tick_ms;

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ Extension words @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
WORD FLAG_INTERPRET_COMPILE, "extension_test"
    //ldr r0, =tick_ms
    //ldr r0, [r0]
    nop
    nop
    bx lr
