#ifndef NET_H
#define NET_H

#include <stdint.h>

/* Ethernet Header */
struct eth_hdr {
    uint8_t  dest[6];
    uint8_t  src[6];
    uint16_t type;
} __attribute__((packed));

/* ARP Header */
struct arp_hdr {
    uint16_t hw_type;
    uint16_t proto_type;
    uint8_t  hw_len;
    uint8_t  proto_len;
    uint16_t opcode;
    uint8_t  sender_mac[6];
    uint32_t sender_ip;
    uint8_t  target_mac[6];
    uint32_t target_ip;
} __attribute__((packed));

/* IP Header (IPv4) */
struct ip_hdr {
    uint8_t  ver_ihl;
    uint8_t  tos;
    uint16_t len;
    uint16_t id;
    uint16_t flags_offset;
    uint8_t  ttl;
    uint8_t  proto;
    uint16_t chksum;
    uint32_t src_ip;
    uint32_t dest_ip;
} __attribute__((packed));

/* ICMP Header */
struct icmp_hdr {
    uint8_t  type;
    uint8_t  code;
    uint16_t chksum;
    uint16_t id;
    uint16_t seq;
} __attribute__((packed));

#define ETHTYPE_ARP 0x0608 // 0x0806 in big-endian
#define ETHTYPE_IP  0x0008 // 0x0800 in big-endian

#define ARP_OP_REQUEST 0x0100
#define ARP_OP_REPLY   0x0200

#define IP_PROTO_ICMP  1

#define ICMP_TYPE_ECHO_REQUEST 8
#define ICMP_TYPE_ECHO_REPLY   0

void net_handle_packet(uint8_t *packet, uint32_t len);

#endif
