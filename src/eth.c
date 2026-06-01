#include "eth.h"
#include "uart.h"

#define REG(x) (*(volatile uint32_t *)(x))

void eth_init(uint8_t *mac) {
    uart_println("[ETH] Initializing Ethernet Controller...");

    /* Disable interrupts and receiver/transmitter */
    REG(ETH_MACIM) = 0;
    REG(ETH_MACRCTL) = 0;
    REG(ETH_MACTCTL) = 0;

    /* Set MAC Address */
    /* IA0: bytes 0-3 (little-endian: mac[0] is lowest byte) */
    REG(ETH_MACIA0) = (mac[0]) | (mac[1] << 8) | (mac[2] << 16) | (mac[3] << 24);
    /* IA1: bytes 4-5 */
    REG(ETH_MACIA1) = (mac[4]) | (mac[5] << 8);

    /* Reset RX FIFO and enable receiver with padding and CRC generation */
    REG(ETH_MACRCTL) = MAC_RCTL_RXEN | MAC_RCTL_RSTFIFO;
    REG(ETH_MACTCTL) = MAC_TCTL_TXEN | MAC_TCTL_PADEN | MAC_TCTL_CRC;

    /* Enable RX Interrupt */
    REG(ETH_MACIM) = MAC_INT_RX;

    uart_println("[ETH] Ethernet Initialized.");
}

void eth_send(void *data, uint32_t len) {
    uint8_t *p = (uint8_t *)data;
    uint32_t word;
    
    /* Write length to first 16 bits of FIFO if hardware requires, 
       but for LM3S, data is written directly to MACDATA. */
    
    // For LM3S6965: First write length (low byte, high byte)
    REG(ETH_MACDATA) = (p[0]) | (p[1] << 8);
    
    // Then write the rest of the packet 4 bytes at a time
    for (uint32_t i = 2; i < len; i += 4) {
        word = p[i];
        if (i+1 < len) word |= (p[i+1] << 8);
        if (i+2 < len) word |= (p[i+2] << 16);
        if (i+3 < len) word |= (p[i+3] << 24);
        REG(ETH_MACDATA) = word;
    }

    /* Start transmission */
    REG(ETH_MACTR) = 1;

    // Wait for completion (simple polling for now)
    while (!(REG(ETH_MACRIS) & MAC_INT_TXEMP));
    REG(ETH_MACIACK) = MAC_INT_TXEMP;
}

uint32_t eth_receive(void *buffer, uint32_t max_len) {
    if (!(REG(ETH_MACNPV) & 0xFF)) return 0; // No packets

    uint32_t len_word = REG(ETH_MACDATA);
    uint32_t len = len_word & 0xFFFF; // Packet length is in the first 2 bytes
    
    uint8_t *p = (uint8_t *)buffer;
    
    // Read words from FIFO
    for (uint32_t i = 0; i < len && i < max_len; i += 4) {
        uint32_t word = REG(ETH_MACDATA);
        p[i] = word & 0xFF;
        if (i+1 < len) p[i+1] = (word >> 8) & 0xFF;
        if (i+2 < len) p[i+2] = (word >> 16) & 0xFF;
        if (i+3 < len) p[i+3] = (word >> 24) & 0xFF;
    }

    return len;
}
