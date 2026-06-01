#ifndef LWIPOPTS_H
#define LWIPOPTS_H

#define LWIP_NO_INTTYPES_H      1
#define LWIP_NO_STDINT_H        1

/* NO_SYS == 1: No operating system, use polling or raw interrupts */
#define NO_SYS                  1
#define LWIP_SOCKET             0
#define LWIP_NETCONN            0

/* Memory management */
#define MEM_ALIGNMENT           4
#define MEM_SIZE                16000
#define MEMP_NUM_PBUF           16
#define MEMP_NUM_UDP_PCB        4
#define MEMP_NUM_TCP_PCB        4
#define MEMP_NUM_TCP_PCB_LISTEN 4
#define MEMP_NUM_TCP_SEG        16
#define MEMP_NUM_SYS_TIMEOUT    8

/* Pbuf options */
#define PBUF_POOL_SIZE          16
#define PBUF_POOL_BUFSIZE       512

/* TCP/IP options */
#define LWIP_TCP                1
#define TCP_MSS                 536
#define TCP_WND                 (4 * TCP_MSS)
#define TCP_SND_BUF             (2 * TCP_MSS)

/* Network interface options */
#define LWIP_ARP                1
#define LWIP_ETHERNET           1
#define LWIP_ICMP               1
#define LWIP_UDP                1
#define LWIP_DHCP               1
#define LWIP_NETIF_HOSTNAME     1
#define LWIP_DNS                1

#define LWIP_HAVE_SLIPIF        0
#define PPP_SUPPORT             0

/* Debugging */
#define LWIP_DEBUG              1
#define ETHARP_DEBUG            LWIP_DBG_OFF
#define NETIF_DEBUG             LWIP_DBG_ON
#define PBUF_DEBUG              LWIP_DBG_OFF
#define API_LIB_DEBUG           LWIP_DBG_OFF
#define API_MSG_DEBUG           LWIP_DBG_OFF
#define SOCKETS_DEBUG           LWIP_DBG_OFF
#define ICMP_DEBUG              LWIP_DBG_OFF
#define INET_DEBUG              LWIP_DBG_OFF
#define IP_DEBUG                LWIP_DBG_OFF
#define IP_REASS_DEBUG          LWIP_DBG_OFF
#define RAW_DEBUG               LWIP_DBG_OFF
#define MEM_DEBUG               LWIP_DBG_OFF
#define MEMP_DEBUG              LWIP_DBG_OFF
#define SYS_DEBUG               LWIP_DBG_OFF
#define TCP_DEBUG               LWIP_DBG_OFF
#define TCP_INPUT_DEBUG         LWIP_DBG_OFF
#define TCP_OUTPUT_DEBUG        LWIP_DBG_OFF
#define TCP_RTO_DEBUG           LWIP_DBG_OFF
#define TCP_CWND_DEBUG          LWIP_DBG_OFF
#define TCP_WND_DEBUG           LWIP_DBG_OFF
#define TCP_FR_DEBUG            LWIP_DBG_OFF
#define TCP_QLEN_DEBUG          LWIP_DBG_OFF
#define TCP_RST_DEBUG           LWIP_DBG_OFF
#define DHCP_DEBUG              LWIP_DBG_OFF
#define UDP_DEBUG               LWIP_DBG_OFF

#endif
