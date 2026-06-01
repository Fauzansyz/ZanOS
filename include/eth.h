#ifndef ETH_H
#define ETH_H

#include <stdint.h>

/* Ethernet Controller Register Offsets (LM3S6965) */
#define ETH_BASE          0x40048000

#define ETH_MACRIS        (ETH_BASE + 0x000) // Interrupt Status
#define ETH_MACIACK       (ETH_BASE + 0x008) // Interrupt Acknowledge
#define ETH_MACIM         (ETH_BASE + 0x004) // Interrupt Mask
#define ETH_MACRCTL       (ETH_BASE + 0x00C) // Receive Control
#define ETH_MACTCTL       (ETH_BASE + 0x010) // Transmit Control
#define ETH_MACDATA       (ETH_BASE + 0x014) // Data Register
#define ETH_MACIA0        (ETH_BASE + 0x018) // MAC Address 0
#define ETH_MACIA1        (ETH_BASE + 0x01C) // MAC Address 1
#define ETH_MACTR         (ETH_BASE + 0x038) // Transmission Request
#define ETH_MACNPV        (ETH_BASE + 0x020) // Number of Packets in FIFO

/* RCTL bits */
#define MAC_RCTL_RSTFIFO  0x00000010
#define MAC_RCTL_BADCRC   0x00000008
#define MAC_RCTL_PRMS     0x00000004
#define MAC_RCTL_AMUL     0x00000002
#define MAC_RCTL_RXEN     0x00000001

/* TCTL bits */
#define MAC_TCTL_PADEN    0x00000004
#define MAC_TCTL_CRC      0x00000002
#define MAC_TCTL_TXEN     0x00000001

/* Interrupt bits */
#define MAC_INT_TXER      0x00000080
#define MAC_INT_TXEMP     0x00000040
#define MAC_INT_FOV       0x00000020
#define MAC_INT_RXER      0x00000010
#define MAC_INT_MDINT     0x00000008
#define MAC_INT_PHY       0x00000004
#define MAC_INT_TX        0x00000002
#define MAC_INT_RX        0x00000001

void eth_init(uint8_t *mac);
void eth_send(void *data, uint32_t len);
uint32_t eth_receive(void *buffer, uint32_t max_len);

#endif
