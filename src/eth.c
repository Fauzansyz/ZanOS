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
    
    /* Write length (low byte first, then high byte) */
    REG(ETH_MACDATA) = (len & 0xFF) | (((len >> 8) & 0xFF) << 8);
    
    /* Write the packet 4 bytes at a time */
    for (uint32_t i = 0; i < len; i += 4) {
        word = p[i];
        if (i+1 < len) word |= (p[i+1] << 8);
        if (i+2 < len) word |= (p[i+2] << 16);
        if (i+3 < len) word |= (p[i+3] << 24);
        REG(ETH_MACDATA) = word;
    }

    /* Start transmission */
    REG(ETH_MACTR) = 1;

    /* Wait for completion with a simple timeout */
    for (volatile int i = 0; i < 1000; i++) {
        if (REG(ETH_MACRIS) & MAC_INT_TXEMP) break;
    }
    REG(ETH_MACIACK) = MAC_INT_TXEMP;
}

uint32_t eth_receive(void *buffer, uint32_t max_len) {
    if (!(REG(ETH_MACNPV) & 0xFF)) return 0; // No packets

    uint32_t len_word = REG(ETH_MACDATA);
    uint32_t len = len_word & 0xFFFF; // Packet length is in the first 2 bytes
    
    // The length word itself is part of the FIFO data, we just read it.
    // Actually, in LM3S, length is bytes 0-1, data starts at byte 2 of the same word?
    // Wait, the datasheet says:
    // "The frame length is contained in the first 16 bits of the first word"
    // "The remaining 16 bits of the first word contain the first 2 bytes of the destination MAC"
    
    uint8_t *p = (uint8_t *)buffer;
    uint32_t stored_len = 0;
    
    // Process the first word (which we already read)
    if (max_len >= 2) {
        p[0] = (len_word >> 16) & 0xFF;
        p[1] = (len_word >> 24) & 0xFF;
        stored_len = 2;
    }
    
    // Calculate how many more words to read
    // The length includes the 2-byte length field itself! 
    // So if len is 64, there are 64 - 2 = 62 bytes of data.
    // 62 bytes minus the 2 bytes we just extracted = 60 bytes left.
    // 60 bytes = 15 words.
    
    uint32_t bytes_to_read = len > 2 ? len - 2 : 0;
    uint32_t words_to_read = (bytes_to_read + 3) / 4;
    
    for (uint32_t i = 0; i < words_to_read; i++) {
        uint32_t word = REG(ETH_MACDATA);
        
        for (int j = 0; j < 4; j++) {
            if (stored_len < bytes_to_read + 2 && stored_len < max_len) {
                p[stored_len++] = (word >> (j * 8)) & 0xFF;
            }
        }
    }

    return stored_len;
}
