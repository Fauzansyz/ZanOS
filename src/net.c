#include "net.h"
#include "eth.h"
#include "uart.h"
#include "strings.h"

/* ZanOS Network Configuration */
static uint8_t  zanos_mac[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02};
static uint32_t zanos_ip  = 0x0201A8C0; // 192.168.1.2 in little-endian

/* Swap bytes for 16-bit networking (Big-Endian) */
static uint16_t ntons(uint16_t n) {
    return ((n & 0xFF) << 8) | ((n & 0xFF00) >> 8);
}

/* Checksum calculation for IP/ICMP */
static uint16_t checksum(void *vdata, uint32_t length) {
    uint16_t *data = (uint16_t *)vdata;
    uint32_t sum = 0;
    for (uint32_t i = 0; i < length / 2; i++) {
        sum += data[i];
    }
    if (length & 1) {
        sum += ((uint8_t *)data)[length - 1];
    }
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    return ~sum;
}

void net_handle_packet(uint8_t *packet, uint32_t len) {
    if (len < sizeof(struct eth_hdr)) return;

    struct eth_hdr *eth = (struct eth_hdr *)packet;

    /* Handle ARP */
    if (eth->type == ETHTYPE_ARP) {
        struct arp_hdr *arp = (struct arp_hdr *)(packet + sizeof(struct eth_hdr));
        
        if (arp->opcode == ARP_OP_REQUEST && arp->target_ip == zanos_ip) {
            uart_println("[NET] Received ARP Request. Sending Reply...");
            
            /* Construct ARP Reply */
            struct eth_hdr eth_reply;
            struct arp_hdr arp_reply;

            memcpy(eth_reply.dest, eth->src, 6);
            memcpy(eth_reply.src, zanos_mac, 6);
            eth_reply.type = ETHTYPE_ARP;

            arp_reply.hw_type = arp->hw_type;
            arp_reply.proto_type = arp->proto_type;
            arp_reply.hw_len = 6;
            arp_reply.proto_len = 4;
            arp_reply.opcode = ARP_OP_REPLY;
            memcpy(arp_reply.sender_mac, zanos_mac, 6);
            arp_reply.sender_ip = zanos_ip;
            memcpy(arp_reply.target_mac, eth->src, 6);
            arp_reply.target_ip = arp->sender_ip;

            uint8_t reply_packet[sizeof(struct eth_hdr) + sizeof(struct arp_hdr)];
            memcpy(reply_packet, &eth_reply, sizeof(struct eth_hdr));
            memcpy(reply_packet + sizeof(struct eth_hdr), &arp_reply, sizeof(struct arp_hdr));

            eth_send(reply_packet, sizeof(reply_packet));
        }
    } 
    /* Handle IP */
    else if (eth->type == ETHTYPE_IP) {
        struct ip_hdr *ip = (struct ip_hdr *)(packet + sizeof(struct eth_hdr));
        
        if (ip->dest_ip == zanos_ip && ip->proto == IP_PROTO_ICMP) {
            struct icmp_hdr *icmp = (struct icmp_hdr *)(packet + sizeof(struct eth_hdr) + sizeof(struct ip_hdr));
            
            if (icmp->type == ICMP_TYPE_ECHO_REQUEST) {
                uart_println("[NET] Received Ping (ICMP Echo). Sending Reply...");
                
                /* Prepare IP Reply */
                ip->dest_ip = ip->src_ip;
                ip->src_ip = zanos_ip;
                ip->chksum = 0;
                ip->chksum = checksum(ip, sizeof(struct ip_hdr));

                /* Prepare ICMP Reply */
                icmp->type = ICMP_TYPE_ECHO_REPLY;
                icmp->chksum = 0;
                uint32_t icmp_len = ntons(ip->len) - sizeof(struct ip_hdr);
                icmp->chksum = checksum(icmp, icmp_len);

                /* Reuse the same packet for simplicity (Ethernet header swap) */
                memcpy(eth->dest, eth->src, 6);
                memcpy(eth->src, zanos_mac, 6);

                eth_send(packet, len);
            }
        }
    }
}
