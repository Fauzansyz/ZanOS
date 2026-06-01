.syntax unified
.cpu cortex-m3
.thumb

.section .isr_vector, "a", %progbits
.global __isr_vector
__isr_vector:
    .word _estack             /* 0: Initial Stack Pointer */
    .word reset_handler + 1   /* 1: Reset Handler */
    .word nmi_handler + 1     /* 2: NMI Handler */
    .word hard_fault_handler + 1 /* 3: Hard Fault Handler */
    .word mem_manage_handler + 1 /* 4: MPU Fault Handler */
    .word bus_fault_handler + 1  /* 5: Bus Fault Handler */
    .word usage_fault_handler + 1/* 6: Usage Fault Handler */
    .word 0                   /* 7: Reserved */
    .word 0                   /* 8: Reserved */
    .word 0                   /* 9: Reserved */
    .word 0                   /* 10: Reserved */
    .word svc_handler + 1     /* 11: SVCall Handler */
    .word debug_mon_handler + 1  /* 12: Debug Monitor Handler */
    .word 0                   /* 13: Reserved */
    .word pendsv_handler + 1  /* 14: PendSV Handler */
    .word systick_handler + 1 /* 15: SysTick Handler */

    /* External Interrupts (IRQs) */
    .rept 42
    .word default_handler + 1
    .endr
    .word ethernet_handler + 1 /* 42: Ethernet Interrupt (Exception 58) */
    
    .rept 100 /* Fill the rest with default handlers up to a reasonable limit */
    .word default_handler + 1
    .endr

.equ _estack, 0x20001000

.section .text
.global reset_handler
reset_handler:
    ldr   sp, =_estack
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
