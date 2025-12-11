.syntax unified
    .cpu cortex-m3
    .thumb
    .section .isr_vector, "a", %progbits
    .word _estack             /* initial stack pointer */
    .word reset_handler + 1   /* reset handler (LSB=1 -> Thumb) */

    .equ _estack, 0x20001000

    .section .text
    .global reset_handler
reset_handler:
    /* load SP from vector table constant (redundant but explicit) */
    ldr   sp, =_estack
    bl    main
hang:
    b     hang
