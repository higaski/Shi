/***************************************************************************//**
@ Peripheral memory map
 ******************************************************************************/
@ Cortex-M4
.equ SCS_BASE, 0xE000E000               @ System Control Space Base Address
.equ ITM_BASE, 0xE0000000               @ ITM Base Address
.equ DWT_BASE, 0xE0001000               @ DWT Base Address
.equ TPI_BASE, 0xE0040000               @ TPI Base Address
.equ CoreDebug_BASE, 0xE000EDF0         @ Core Debug Base Address
.equ SysTick_BASE, SCS_BASE + 0x0010    @ SysTick Base Address
.equ NVIC_BASE, SCS_BASE + 0x0100       @ NVIC Base Address
.equ SCB_BASE, SCS_BASE + 0x0D00        @ System Control Block Base Address

@ ITM
.equ ITM_PORT, ITM_BASE + 0x0000        @ ITM stimulus port register
.equ ITM_TER, ITM_BASE + 0x0E00         @ ITM trace enable register
.equ ITM_TPR, ITM_BASE + 0x0E40         @ ITM trace privilege register
.equ ITM_TCR, ITM_BASE + 0x0E80         @ ITM trace control register
.equ ITM_IWR, ITM_BASE + 0x0EF8         @ ITM integration write register
.equ ITM_IRR, ITM_BASE + 0x0EFC         @ ITM integration read register
.equ ITM_IMCR, ITM_BASE + 0x0F00        @ ITM integration mode control register
.equ ITM_LAR, ITM_BASE + 0x0FB0         @ ITM lock access register
.equ ITM_LSR, ITM_BASE + 0x0FB4         @ ITM lock status register
.equ ITM_PID4, ITM_BASE + 0x0FD0        @ ITM peripheral identification register #4
.equ ITM_PID5, ITM_BASE + 0x0FD4        @ ITM peripheral identification register #5
.equ ITM_PID6, ITM_BASE + 0x0FD8        @ ITM peripheral identification register #6
.equ ITM_PID7, ITM_BASE + 0x0FDC        @ ITM peripheral identification register #7
.equ ITM_PID0, ITM_BASE + 0x0FE0        @ ITM peripheral identification register #0
.equ ITM_PID1, ITM_BASE + 0x0FE4        @ ITM peripheral identification register #1
.equ ITM_PID2, ITM_BASE + 0x0FE8        @ ITM peripheral identification register #2
.equ ITM_PID3, ITM_BASE + 0x0FEC        @ ITM peripheral identification register #3
.equ ITM_CID0, ITM_BASE + 0x0FF0        @ ITM component identification register #0
.equ ITM_CID1, ITM_BASE + 0x0FF4        @ ITM component identification register #1
.equ ITM_CID2, ITM_BASE + 0x0FF8        @ ITM component identification register #2
.equ ITM_CID3, ITM_BASE + 0x0FFC        @ ITM component identification register #3

@ STM32L431
.equ FLASH_SIZE, 0x00040000             @ FLASH size (256 KBytes)
.equ SRAM2_SIZE, 0x00004000             @ SRAM2 size (16 KBytes)
.equ PERIPH_BASE, 0x40000000            @ Peripheral base address

.equ APB1PERIPH_BASE, PERIPH_BASE
.equ APB2PERIPH_BASE, PERIPH_BASE + 0x00010000
.equ AHB1PERIPH_BASE, PERIPH_BASE + 0x00020000
.equ AHB2PERIPH_BASE, PERIPH_BASE + 0x08000000

@ APB1 peripherals
.equ TIM2_BASE, APB1PERIPH_BASE + 0x0000
.equ TIM3_BASE, APB1PERIPH_BASE + 0x0400
.equ TIM4_BASE, APB1PERIPH_BASE + 0x0800
.equ TIM5_BASE, APB1PERIPH_BASE + 0x0C00
.equ TIM6_BASE, APB1PERIPH_BASE + 0x1000
.equ TIM7_BASE, APB1PERIPH_BASE + 0x1400
.equ LCD_BASE, APB1PERIPH_BASE + 0x2400
.equ RTC_BASE, APB1PERIPH_BASE + 0x2800
.equ WWDG_BASE, APB1PERIPH_BASE + 0x2C00
.equ IWDG_BASE, APB1PERIPH_BASE + 0x3000
.equ SPI2_BASE, APB1PERIPH_BASE + 0x3800
.equ SPI3_BASE, APB1PERIPH_BASE + 0x3C00
.equ USART2_BASE, APB1PERIPH_BASE + 0x4400
.equ USART3_BASE, APB1PERIPH_BASE + 0x4800
.equ UART4_BASE, APB1PERIPH_BASE + 0x4C00
.equ UART5_BASE, APB1PERIPH_BASE + 0x5000
.equ I2C1_BASE, APB1PERIPH_BASE + 0x5400
.equ I2C2_BASE, APB1PERIPH_BASE + 0x5800
.equ I2C3_BASE, APB1PERIPH_BASE + 0x5C00
.equ CAN1_BASE, APB1PERIPH_BASE + 0x6400
.equ PWR_BASE, APB1PERIPH_BASE + 0x7000
.equ DAC_BASE, APB1PERIPH_BASE + 0x7400
.equ DAC1_BASE, APB1PERIPH_BASE + 0x7400
.equ OPAMP_BASE, APB1PERIPH_BASE + 0x7800
.equ OPAMP1_BASE, APB1PERIPH_BASE + 0x7800
.equ OPAMP2_BASE, APB1PERIPH_BASE + 0x7810
.equ LPTIM1_BASE, APB1PERIPH_BASE + 0x7C00
.equ LPUART1_BASE, APB1PERIPH_BASE + 0x8000
.equ SWPMI1_BASE, APB1PERIPH_BASE + 0x8800
.equ LPTIM2_BASE, APB1PERIPH_BASE + 0x9400

@ APB2 peripherals
.equ SYSCFG_BASE, APB2PERIPH_BASE + 0x0000
.equ VREFBUF_BASE, APB2PERIPH_BASE + 0x0030
.equ COMP1_BASE, APB2PERIPH_BASE + 0x0200
.equ COMP2_BASE, APB2PERIPH_BASE + 0x0204
.equ EXTI_BASE, APB2PERIPH_BASE + 0x0400
.equ FIREWALL_BASE, APB2PERIPH_BASE + 0x1C00
.equ SDMMC1_BASE, APB2PERIPH_BASE + 0x2800
.equ TIM1_BASE, APB2PERIPH_BASE + 0x2C00
.equ SPI1_BASE, APB2PERIPH_BASE + 0x3000
.equ TIM8_BASE, APB2PERIPH_BASE + 0x3400
.equ USART1_BASE, APB2PERIPH_BASE + 0x3800
.equ TIM15_BASE, APB2PERIPH_BASE + 0x4000
.equ TIM16_BASE, APB2PERIPH_BASE + 0x4400
.equ TIM17_BASE, APB2PERIPH_BASE + 0x4800
.equ SAI1_BASE, APB2PERIPH_BASE + 0x5400
.equ SAI1_Block_A_BASE, SAI1_BASE + 0x004
.equ SAI1_Block_B_BASE, SAI1_BASE + 0x024
.equ SAI2_BASE, APB2PERIPH_BASE + 0x5800
.equ SAI2_Block_A_BASE, SAI2_BASE + 0x004
.equ SAI2_Block_B_BASE, SAI2_BASE + 0x024
.equ DFSDM1_BASE, APB2PERIPH_BASE + 0x6000
.equ DFSDM1_Channel0_BASE, DFSDM1_BASE + 0x00
.equ DFSDM1_Channel1_BASE, DFSDM1_BASE + 0x20
.equ DFSDM1_Channel2_BASE, DFSDM1_BASE + 0x40
.equ DFSDM1_Channel3_BASE, DFSDM1_BASE + 0x60
.equ DFSDM1_Channel4_BASE, DFSDM1_BASE + 0x80
.equ DFSDM1_Channel5_BASE, DFSDM1_BASE + 0xA0
.equ DFSDM1_Channel6_BASE, DFSDM1_BASE + 0xC0
.equ DFSDM1_Channel7_BASE, DFSDM1_BASE + 0xE0
.equ DFSDM1_Filter0_BASE, DFSDM1_BASE + 0x100
.equ DFSDM1_Filter1_BASE, DFSDM1_BASE + 0x180
.equ DFSDM1_Filter2_BASE, DFSDM1_BASE + 0x200
.equ DFSDM1_Filter3_BASE, DFSDM1_BASE + 0x280

@ AHB1 peripherals
.equ DMA1_BASE, AHB1PERIPH_BASE
.equ DMA2_BASE, AHB1PERIPH_BASE + 0x0400
.equ RCC_BASE, AHB1PERIPH_BASE + 0x1000
.equ FLASH_R_BASE, AHB1PERIPH_BASE + 0x2000
.equ CRC_BASE, AHB1PERIPH_BASE + 0x3000
.equ TSC_BASE, AHB1PERIPH_BASE + 0x4000

.equ DMA1_Channel1_BASE, DMA1_BASE + 0x0008
.equ DMA1_Channel2_BASE, DMA1_BASE + 0x001C
.equ DMA1_Channel3_BASE, DMA1_BASE + 0x0030
.equ DMA1_Channel4_BASE, DMA1_BASE + 0x0044
.equ DMA1_Channel5_BASE, DMA1_BASE + 0x0058
.equ DMA1_Channel6_BASE, DMA1_BASE + 0x006C
.equ DMA1_Channel7_BASE, DMA1_BASE + 0x0080
.equ DMA1_CSELR_BASE, DMA1_BASE + 0x00A8

.equ DMA2_Channel1_BASE, DMA2_BASE + 0x0008
.equ DMA2_Channel2_BASE, DMA2_BASE + 0x001C
.equ DMA2_Channel3_BASE, DMA2_BASE + 0x0030
.equ DMA2_Channel4_BASE, DMA2_BASE + 0x0044
.equ DMA2_Channel5_BASE, DMA2_BASE + 0x0058
.equ DMA2_Channel6_BASE, DMA2_BASE + 0x006C
.equ DMA2_Channel7_BASE, DMA2_BASE + 0x0080
.equ DMA2_CSELR_BASE, DMA2_BASE + 0x00A8

@ AHB2 peripherals
.equ GPIOA_BASE, AHB2PERIPH_BASE + 0x0000
.equ GPIOB_BASE, AHB2PERIPH_BASE + 0x0400
.equ GPIOC_BASE, AHB2PERIPH_BASE + 0x0800
.equ GPIOD_BASE, AHB2PERIPH_BASE + 0x0C00
.equ GPIOE_BASE, AHB2PERIPH_BASE + 0x1000
.equ GPIOF_BASE, AHB2PERIPH_BASE + 0x1400
.equ GPIOG_BASE, AHB2PERIPH_BASE + 0x1800
.equ GPIOH_BASE, AHB2PERIPH_BASE + 0x1C00

.equ USBOTG_BASE, AHB2PERIPH_BASE + 0x08000000

.equ ADC1_BASE, AHB2PERIPH_BASE + 0x08040000
.equ ADC2_BASE, AHB2PERIPH_BASE + 0x08040100
.equ ADC3_BASE, AHB2PERIPH_BASE + 0x08040200
.equ ADC123_COMMON_BASE, AHB2PERIPH_BASE + 0x08040300

.equ RNG_BASE, AHB2PERIPH_BASE + 0x08060800

@ FLASH
.equ FLASH_ACR, FLASH_R_BASE + 0x00         @ FLASH access control register
.equ FLASH_PDKEYR, FLASH_R_BASE + 0x04      @ FLASH power down key register
.equ FLASH_KEYR, FLASH_R_BASE + 0x08        @ FLASH key register
.equ FLASH_OPTKEYR, FLASH_R_BASE + 0x0C     @ FLASH option key register
.equ FLASH_SR, FLASH_R_BASE + 0x10          @ FLASH status register
    .equ FLASH_SR_EOP, 0x00000001               @ End of operation
    .equ FLASH_SR_OP_ERR, 0x00000002            @ Operation error
    .equ FLASH_SR_PROG_ERR, 0x00000008          @ Programming error
    .equ FLASH_SR_WRP_ERR, 0x00000010           @ Write protection error
    .equ FLASH_SR_PGA_ERR, 0x00000020           @ Programming alignment error
    .equ FLASH_SR_SIZ_ERR, 0x00000040           @ Size error
    .equ FLASH_SR_PGS_ERR, 0x00000080           @ Programming sequence error
    .equ FLASH_SR_MISS_ERR, 0x00000100          @ Fast programming data miss error
    .equ FLASH_SR_FAST_ERR, 0x00000200          @ Fast programming error
    .equ FLASH_SR_RD_ERR, 0x00004000            @ PCROP read error
    .equ FLASH_SR_OPTV_ERR, 0x00008000          @ Option validity error
    .equ FLASH_SR_BSY, 0x00010000               @ Busy
    .equ FLASH_SR_P_EMPTY, 0x00020000           @ Programm empty
.equ FLASH_CR, FLASH_R_BASE + 0x14          @ FLASH control register
    .equ FLASH_CR_PG, 0x00000001                @ Programming
    .equ FLASH_CR_PER, 0x00000002               @ Page erase
    .equ FLASH_CR_MER1, 0x00000004              @ Mass erase
    .equ FLASH_CR_PNG0, 0x00000008              @ Pange number selection
    .equ FLASH_CR_PNG1, 0x00000010
    .equ FLASH_CR_PNG2, 0x00000020
    .equ FLASH_CR_PNG3, 0x00000040
    .equ FLASH_CR_PNG4, 0x00000080
    .equ FLASH_CR_PNG5, 0x00000100
    .equ FLASH_CR_PNG6, 0x00000200
    .equ FLASH_CR_PNG7, 0x00000400
    .equ FLASH_CR_STRT, 0x00010000              @ Start
    .equ FLASH_CR_OPT_STRT, 0x00020000          @ Options modification start
    .equ FLASH_CR_FSTPG, 0x00040000             @ Fast programming
    .equ FLASH_CR_EOP_IE, 0x01000000            @ End of operation interrupt enable
    .equ FLASH_CR_ERR_IE, 0x02000000            @ Error interrupt enable
    .equ FLASH_CR_RD_ERRIE, 0x04000000          @ PCROP read error interrupt enable
    .equ FLASH_CR_OBL_LAUNCH, 0x08000000        @ Force the option byte loading
    .equ FLASH_CR_OPT_LOCK, 0x40000000          @ Options lock
    .equ FLASH_CR_LOCK, 0x80000000              @ Lock
.equ FLASH_ECCR, FLASH_R_BASE + 0x18        @ FLASH ECC register
.equ FLASH_OPTR, FLASH_R_BASE + 0x20        @ FLASH option register
.equ FLASH_PCROP1SR, FLASH_R_BASE + 0x24    @ FLASH bank1 PCROP start address register
.equ FLASH_PCROP1ER, FLASH_R_BASE + 0x28    @ FLASH bank1 PCROP end address register
.equ FLASH_WRP1AR, FLASH_R_BASE + 0x2C      @ FLASH bank1 WRP area A address register
.equ FLASH_WRP1BR, FLASH_R_BASE + 0x30      @ FLASH bank1 WRP area B address register
.equ FLASH_PCROP2SR, FLASH_R_BASE + 0x44    @ FLASH bank2 PCROP start address register
.equ FLASH_PCROP2ER, FLASH_R_BASE + 0x48    @ FLASH bank2 PCROP end address register
.equ FLASH_WRP2AR, FLASH_R_BASE + 0x4C      @ FLASH bank2 WRP area A address register
.equ FLASH_WRP2BR, FLASH_R_BASE + 0x50      @ FLASH bank2 WRP area B address register

.equ FLASH_KEY1, 0x45670123             @ Flash key1
.equ FLASH_KEY2, 0xCDEF89AB             @ Flash key2
/** @}
 */
