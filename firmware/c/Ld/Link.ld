ENTRY( _start )

__stack_size = 2048;

PROVIDE( _stack_size = __stack_size );


MEMORY
{
/* CH32V20x_D6 - CH32V203F6-CH32V203G6-CH32V203K6-CH32V203C6 */
/*
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 32K
    RAM (xrw) : ORIGIN = 0x20000000, LENGTH = 10K
*/

/* CH32V20x_D6 - CH32V203K8-CH32V203C8-CH32V203G8-CH32V203F8 */
/**/
    /*
     * These parts actually contain a 256K flash wafer wire-bonded to the core!
     *
     * The first 224K is in the main flash area at 0x0800_0000
     * The next 28K is in the boot-loader flash area at 0x1FFF_8000
     * The last 4K is used by the vendor-bytes, option-bytes, and "reserved"
     *
     * The first 64K of the first 224K is "zero-wait", i.e. where your program
     * should live when executing from flash, and works at full CPU speed with
     * no stalling. The remainder must be accessed at no more than 60MHz,
     * with the flash clock generated from the CPU clock with a /2 divider.
     * This means, you must not clock the CPU at 144MHz if you want to use all
     * the available flash! Even worse, if you want to use USB as well as all
     * the flash, your CPU clock will be limited to 96MHz.
     */

    /* zero-wait flash */
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 64K
    /* remainder of flash for code */
    FLASH_SLOW (rx) : ORIGIN = 0x00000000 + 64K, LENGTH = 128K
    /* flash reserved for data storage area */
    FLASH_DATA (rw) : ORIGIN = 0x00000000 + 64K + 128K, LENGTH = 32K
    /* internal SRAM */
    RAM (xrw) : ORIGIN = 0x20000000, LENGTH = 20K


/* CH32V20x_D8 - CH32V203RB
   CH32V20x_D8W - CH32V208x
   FLASH + RAM supports the following configuration
   FLASH-128K + RAM-64K
   FLASH-144K + RAM-48K
   FLASH-160K + RAM-32K

    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 160K
    RAM (xrw) : ORIGIN = 0x20000000, LENGTH = 32K
*/
}


SECTIONS
{

    .init :
    {
        _sinit = .;
        . = ALIGN(4);
        KEEP(*(SORT_NONE(.init)))
        . = ALIGN(4);
        _einit = .;
    } >FLASH AT>FLASH

    .vector :
    {
        *(.vector);
        . = ALIGN(64);
    } >FLASH AT>FLASH

    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text.*)
        *(.glue_7)
        *(.glue_7t)
        *(.gnu.linkonce.t.*)
        . = ALIGN(4);
    } >FLASH AT>FLASH

    .fini :
    {
        KEEP(*(SORT_NONE(.fini)))
        . = ALIGN(4);
    } >FLASH AT>FLASH

    PROVIDE( _etext = . );
    PROVIDE( _eitcm = . );

    .preinit_array  :
    {
      PROVIDE_HIDDEN (__preinit_array_start = .);
      KEEP (*(.preinit_array))
      PROVIDE_HIDDEN (__preinit_array_end = .);
    } >FLASH AT>FLASH

    .init_array     :
    {
      PROVIDE_HIDDEN (__init_array_start = .);
      KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
      KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
      PROVIDE_HIDDEN (__init_array_end = .);
    } >FLASH AT>FLASH

    .fini_array     :
    {
      PROVIDE_HIDDEN (__fini_array_start = .);
      KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
      KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
      PROVIDE_HIDDEN (__fini_array_end = .);
    } >FLASH AT>FLASH

    .ctors          :
    {
      /* gcc uses crtbegin.o to find the start of
         the constructors, so we make sure it is
         first.  Because this is a wildcard, it
         doesn't matter if the user does not
         actually link against crtbegin.o; the
         linker won't look for a file to match a
         wildcard.  The wildcard also means that it
         doesn't matter which directory crtbegin.o
         is in.  */
      KEEP (*crtbegin.o(.ctors))
      KEEP (*crtbegin?.o(.ctors))
      /* We don't want to include the .ctor section from
         the crtend.o file until after the sorted ctors.
         The .ctor section from the crtend file contains the
         end of ctors marker and it must be last */
      KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
      KEEP (*(SORT(.ctors.*)))
      KEEP (*(.ctors))
    } >FLASH AT>FLASH

    .dtors          :
    {
      KEEP (*crtbegin.o(.dtors))
      KEEP (*crtbegin?.o(.dtors))
      KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
      KEEP (*(SORT(.dtors.*)))
      KEEP (*(.dtors))
    } >FLASH AT>FLASH

    .dalign :
    {
        . = ALIGN(4);
        PROVIDE(_data_vma = .);
    } >RAM AT>FLASH_SLOW

    .dlalign :
    {
        . = ALIGN(4);
        PROVIDE(_data_lma = .);
    } >FLASH AT>FLASH

    .data :
    {
        *(.gnu.linkonce.r.*)
        *(.data .data.*)
        *(.gnu.linkonce.d.*)
        . = ALIGN(8);
        PROVIDE( __global_pointer$ = . + 0x800 );
        *(.sdata .sdata.*)
        *(.sdata2.*)
        *(.gnu.linkonce.s.*)
        . = ALIGN(8);
        *(.srodata.cst16)
        *(.srodata.cst8)
        *(.srodata.cst4)
        *(.srodata.cst2)
        *(.srodata .srodata.*)
        *(.ramfunc .ramfunc.*)
        . = ALIGN(4);
        PROVIDE( _edata = .);
    } >RAM AT>FLASH_SLOW

    .slow :
    {
        *(.slowfunc .slowfunc.*)
        *(.slowconst .slowconst.*)
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } >FLASH_SLOW AT>FLASH_SLOW

    .bss :
    {
        . = ALIGN(4);
        PROVIDE( _sbss = .);
          *(.sbss*)
        *(.gnu.linkonce.sb.*)
        *(.bss*)
         *(.gnu.linkonce.b.*)
        *(COMMON*)
        . = ALIGN(4);
        PROVIDE( _ebss = .);
    } >RAM AT>FLASH_SLOW

    PROVIDE( _end = _ebss);
    PROVIDE( end = . );

    .stack ORIGIN(RAM) + LENGTH(RAM) - __stack_size :
    {
        PROVIDE( _heap_end = . );
        . = ALIGN(4);
        PROVIDE(_susrstack = . );
        . = . + __stack_size;
        PROVIDE( _eusrstack = .);
        __freertos_irq_stack_top = .;
    } >RAM

}
