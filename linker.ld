ENTRY(reset_handler)

MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 256K
    RAM  (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
}

SECTIONS
{
    .text :
    {
        KEEP(*(.isr_vector))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
    } > FLASH

    .ARM.exidx :
    {
        *(.ARM.exidx*)
    } > FLASH

    _etext = LOADADDR(.data);

    .data : AT(LOADADDR(.ARM.exidx) + SIZEOF(.ARM.exidx))
    {
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > RAM

    .bss :
    {
        _sbss = .;
        __bss_start__ = _sbss;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
        __bss_end__ = _ebss;
    } > RAM

    .heap (NOLOAD) :
    {
        . = ALIGN(4);
        _sheap = .;
        . = . + (8 * 1024); /* 8KB Heap */
        _eheap = .;
    } > RAM

    _estack = ORIGIN(RAM) + LENGTH(RAM);
}
