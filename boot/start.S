.syntax unified
.cpu cortex-m3
.thumb

.section .isr_vector, "a", %progbits
.global __isr_vector
__isr_vector:
    .word _estack             /* 0: Initial Stack Pointer */
    .word reset_handler       /* 1: Reset Handler */
    .word nmi_handler         /* 2: NMI Handler */
    .word hard_fault_handler  /* 3: Hard Fault Handler */
    .word mem_manage_handler  /* 4: MPU Fault Handler */
    .word bus_fault_handler   /* 5: Bus Fault Handler */
    .word usage_fault_handler /* 6: Usage Fault Handler */
    .word 0                   /* 7: Reserved */
    .word 0                   /* 8: Reserved */
    .word 0                   /* 9: Reserved */
    .word 0                   /* 10: Reserved */
    .word svc_handler         /* 11: SVCall Handler */
    .word debug_mon_handler   /* 12: Debug Monitor Handler */
    .word 0                   /* 13: Reserved */
    .word pendsv_handler      /* 14: PendSV Handler */
    .word systick_handler     /* 15: SysTick Handler */

    /* External Interrupts (IRQs) */
    .rept 42
    .word default_handler
    .endr
    .word ethernet_handler    /* 42: Ethernet Interrupt (Exception 58) */
    
    .rept 100 /* Fill the rest with default handlers up to a reasonable limit */
    .word default_handler
    .endr

.section .text
.thumb_func
.global reset_handler
reset_handler:
    /* Load SP */
    ldr   sp, =_estack

    /* Copy .data from Flash to RAM */
    ldr   r0, =_etext
    ldr   r1, =_sdata
    ldr   r2, =_edata
copy_data:
    cmp   r1, r2
    bge   end_copy_data
    ldr   r3, [r0], #4
    str   r3, [r1], #4
    b     copy_data
end_copy_data:

    /* Zero out .bss */
    ldr   r1, =_sbss
    ldr   r2, =_ebss
    mov   r3, #0
zero_bss:
    cmp   r1, r2
    bge   end_zero_bss
    str   r3, [r1], #4
    b     zero_bss
end_zero_bss:

    bl    main
hang:
    b     hang

.thumb_func
default_handler:
    b .

/* Alias all fault handlers to default_handler unless defined elsewhere */
.weak nmi_handler
.thumb_set nmi_handler, default_handler
.weak hard_fault_handler
.thumb_set hard_fault_handler, default_handler
.weak mem_manage_handler
.thumb_set mem_manage_handler, default_handler
.weak bus_fault_handler
.thumb_set bus_fault_handler, default_handler
.weak usage_fault_handler
.thumb_set usage_fault_handler, default_handler
.weak svc_handler
.thumb_set svc_handler, default_handler
.weak debug_mon_handler
.thumb_set debug_mon_handler, default_handler
.weak pendsv_handler
.thumb_set pendsv_handler, default_handler
.weak systick_handler
.thumb_set systick_handler, default_handler
.weak ethernet_handler
.thumb_set ethernet_handler, default_handler
