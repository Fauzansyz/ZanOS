ENTRY(reset_handler)

SECTIONS
{
  /* place vector table at 0x0 */
  . = 0x00000000;
  .isr_vector : { *(.isr_vector) }

  /* place code after vector table */
  . = ALIGN(4);
  .text : { *(.text*) }
  .rodata : { *(.rodata*) }

  . = ALIGN(4);
  .data : { *(.data*) }
  .bss : { *(.bss*) *(COMMON) }
}
