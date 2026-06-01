#include "uart.h"
#include "eth.h"
#include <stdint.h>
#include <stdbool.h>

volatile bool eth_rx_flag = false;

void ethernet_handler(void) {
    eth_rx_flag = true;
    /* Acknowledge the interrupt */
    (*(volatile uint32_t *)(ETH_MACIACK)) = MAC_INT_RX;
}

void hard_fault_handler(void) {
    uart_println("!!! HARD FAULT !!!");
    while(1);
}

extern void sys_inc_ticks(uint32_t ms);

void systick_handler(void) {
    /* increment ticks */
    sys_inc_ticks(10);
}

/* Global interrupt enable */
void interrupts_enable(void) {
    __asm volatile ("cpsie i" : : : "memory");
}

/* Global interrupt disable */
void interrupts_disable(void) {
    __asm volatile ("cpsid i" : : : "memory");
}
