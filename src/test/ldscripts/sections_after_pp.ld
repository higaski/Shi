__stack = ORIGIN(RAM) + LENGTH(RAM);
_estack = __stack;
__Main_Stack_Size = 1024 ;
PROVIDE ( _Main_Stack_Size = __Main_Stack_Size ) ;
__Main_Stack_Limit = __stack - __Main_Stack_Size ;
PROVIDE ( _Main_Stack_Limit = __Main_Stack_Limit ) ;
_Minimum_Stack_Size = 256 ;
PROVIDE ( _Heap_Begin = _end_noinit ) ;
PROVIDE ( _Heap_Limit = __stack - __Main_Stack_Size ) ;
ENTRY(_start)
SECTIONS
{
    .isr_vector : ALIGN(4)
    {
        FILL(0xFF)
        __vectors_start = ABSOLUTE(.) ;
        __vectors_start__ = ABSOLUTE(.) ;
        KEEP(*(.isr_vector))
  KEEP(*(.cfmconfig))
        *(.after_vectors .after_vectors.*)
    } >FLASH
    .inits : ALIGN(4)
    {
        __data_regions_array_start = .;
        LONG(LOADADDR(.data));
        LONG(ADDR(.data));
        LONG(ADDR(.data)+SIZEOF(.data));
        LONG(LOADADDR(.data_CCMRAM));
        LONG(ADDR(.data_CCMRAM));
        LONG(ADDR(.data_CCMRAM)+SIZEOF(.data_CCMRAM));
        __data_regions_array_end = .;
        __bss_regions_array_start = .;
        LONG(ADDR(.bss));
        LONG(ADDR(.bss)+SIZEOF(.bss));
        LONG(ADDR(.bss_CCMRAM));
        LONG(ADDR(.bss_CCMRAM)+SIZEOF(.bss_CCMRAM));
        __bss_regions_array_end = .;
  KEEP(*(.init))
  KEEP(*(.fini))
  . = ALIGN(4);
  PROVIDE_HIDDEN (__preinit_array_start = .);
  KEEP(*(.preinit_array_sysinit .preinit_array_sysinit.*))
  KEEP(*(.preinit_array_platform .preinit_array_platform.*))
  KEEP(*(.preinit_array .preinit_array.*))
  PROVIDE_HIDDEN (__preinit_array_end = .);
  . = ALIGN(4);
  PROVIDE_HIDDEN (__init_array_start = .);
  KEEP(*(SORT(.init_array.*)))
  KEEP(*(.init_array))
  PROVIDE_HIDDEN (__init_array_end = .);
  . = ALIGN(4);
  PROVIDE_HIDDEN (__fini_array_start = .);
  KEEP(*(SORT(.fini_array.*)))
  KEEP(*(.fini_array))
  PROVIDE_HIDDEN (__fini_array_end = .);
    } >FLASH
    .flashtext : ALIGN(4)
    {
        *(.flashtext .flashtext.*)
    } >FLASH
    .text : ALIGN(4)
    {
        *(.text .text.*)
        *(.rodata .rodata.* .constdata .constdata.*)
        *(vtable)
  KEEP(*(.eh_frame*))
        *(.glue_7)
        *(.glue_7t)
    } >FLASH
 .ARM.extab : ALIGN(4)
    {
       *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH
    . = ALIGN(4);
    __exidx_start = .;
    .ARM.exidx : ALIGN(4)
    {
       *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH
    __exidx_end = .;
    . = ALIGN(4);
    _etext = .;
    __etext = .;
    .data_CCMRAM : ALIGN(4)
    {
       FILL(0xFF)
       *(.data.CCMRAM .data.CCMRAM.*)
       . = ALIGN(4) ;
    } > CCMRAM AT>FLASH
    _sidata = LOADADDR(.data);
    .data : ALIGN(4)
    {
     FILL(0xFF)
        _sdata = . ;
        __data_start__ = . ;
  *(.data_begin .data_begin.*)
  *(.data .data.*)
  *(.data_end .data_end.*)
     . = ALIGN(4);
        _edata = . ;
        __data_end__ = . ;
    } >RAM AT>FLASH
    .fill : { FILL(0xFF); . = ORIGIN(FLASH) + LENGTH(FLASH) - 1; BYTE(0xFF) } >FLASH
 .bss_CCMRAM (NOLOAD) : ALIGN(4)
 {
  *(.bss.CCMRAM .bss.CCMRAM.*)
 } > CCMRAM
    .bss (NOLOAD) : ALIGN(4)
    {
        __bss_start__ = .;
        _sbss = .;
        *(.bss_begin .bss_begin.*)
        *(.bss .bss.*)
        *(COMMON)
        *(.bss_end .bss_end.*)
     . = ALIGN(4);
        __bss_end__ = .;
        _ebss = . ;
    } >RAM
    .noinit_CCMRAM (NOLOAD) : ALIGN(4)
    {
        *(.noinit.CCMRAM .noinit.CCMRAM.*)
    } > CCMRAM
    .noinit (NOLOAD) : ALIGN(4)
    {
        _noinit = .;
        *(.noinit .noinit.*)
         . = ALIGN(4) ;
        _end_noinit = .;
    } > RAM
    PROVIDE ( end = _end_noinit );
    PROVIDE ( _end = _end_noinit );
    PROVIDE ( __end = _end_noinit );
    PROVIDE ( __end__ = _end_noinit );
    ._check_stack : ALIGN(4)
    {
        . = . + _Minimum_Stack_Size ;
    } >RAM
    .b1text : ALIGN(4)
    {
        *(.b1text)
        *(.b1rodata)
        *(.b1rodata.*)
    } >FLASHB1
    .eb0text : ALIGN(4)
    {
        *(.eb0text)
        *(.eb0rodata)
        *(.eb0rodata.*)
    } >EXTMEMB0
    .eb1text : ALIGN(4)
    {
        *(.eb1text)
        *(.eb1rodata)
        *(.eb1rodata.*)
    } >EXTMEMB1
    .eb2text : ALIGN(4)
    {
        *(.eb2text)
        *(.eb2rodata)
        *(.eb2rodata.*)
    } >EXTMEMB2
    .eb3text : ALIGN(4)
    {
        *(.eb3text)
        *(.eb3rodata)
        *(.eb3rodata.*)
    } >EXTMEMB3
    .stab 0 : { *(.stab) }
    .stabstr 0 : { *(.stabstr) }
    .stab.excl 0 : { *(.stab.excl) }
    .stab.exclstr 0 : { *(.stab.exclstr) }
    .stab.index 0 : { *(.stab.index) }
    .stab.indexstr 0 : { *(.stab.indexstr) }
    .comment 0 : { *(.comment) }
    .debug 0 : { *(.debug) }
    .line 0 : { *(.line) }
    .debug_srcinfo 0 : { *(.debug_srcinfo) }
    .debug_sfnames 0 : { *(.debug_sfnames) }
    .debug_aranges 0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    .debug_info 0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev 0 : { *(.debug_abbrev) }
    .debug_line 0 : { *(.debug_line) }
    .debug_frame 0 : { *(.debug_frame) }
    .debug_str 0 : { *(.debug_str) }
    .debug_loc 0 : { *(.debug_loc) }
    .debug_macinfo 0 : { *(.debug_macinfo) }
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames 0 : { *(.debug_varnames) }
}
