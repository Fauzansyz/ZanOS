#include "uart.h"
#include "eth.h"
#include "net.h"
#include <stdint.h>

void ethernet_handler(void) {
    uint8_t buffer[1518];
    uint32_t len = eth_receive(buffer, 1518);
    
    if (len > 0) {
        net_handle_packet(buffer, len);
    }

    /* Acknowledge the interrupt */
    (*(volatile uint32_t *)(ETH_MACIACK)) = MAC_INT_RX;
}

void hard_fault_handler(void) {
    uart_println("!!! HARD FAULT !!!");
    while(1);
}

void systick_handler(void) {
}

/* Global interrupt enable */
void interrupts_enable(void) {
    __asm volatile ("cpsie i" : : : "memory");
}

/* Global interrupt disable */
void interrupts_disable(void) {
    __asm volatile ("cpsid i" : : : "memory");
}
