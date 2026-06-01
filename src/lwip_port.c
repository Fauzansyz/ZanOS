#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "netif/etharp.h"
#include "string.h"
#include "eth.h"
#include "uart.h"

#define IFNAME0 'e'
#define IFNAME1 'n'

struct ethernetif {
    /* Add any device-specific state here */
};

/* Low-level output function: sends a pbuf over the wire */
static err_t low_level_output(struct netif *netif, struct pbuf *p) {
    uint8_t buffer[1518];
    uint32_t len = 0;

    /* Copy pbuf chain into a single linear buffer */
    for (struct pbuf *q = p; q != NULL; q = q->next) {
        if (len + q->len > 1518) break;
        memcpy(buffer + len, q->payload, q->len);
        len += q->len;
    }

    eth_send(buffer, len);
    return ERR_OK;
}

/* Low-level input function: called when a packet is received */
void ethernetif_input(struct netif *netif) {
    struct pbuf *p;
    uint8_t buffer[1518];
    uint32_t len = eth_receive(buffer, 1518);

    if (len <= 0) return;

    /* Allocate a pbuf and copy the data into it */
    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
    if (p != NULL) {
        pbuf_take(p, buffer, len);
        /* Full stack processing */
        if (netif->input(p, netif) != ERR_OK) {
            pbuf_free(p);
        }
    }
}

#include "lwip/sys.h"

extern void interrupts_enable(void);
extern void interrupts_disable(void);

static volatile uint32_t sys_ticks = 0;

void sys_inc_ticks(uint32_t ms) {
    sys_ticks += ms;
}

uint32_t sys_now(void) {
    return sys_ticks;
}

sys_prot_t sys_arch_protect(void) {
    interrupts_disable();
    return 0;
}

void sys_arch_unprotect(sys_prot_t pval) {
    (void)pval;
    interrupts_enable();
}

/* Initializer */
err_t ethernetif_init(struct netif *netif) {
    netif->hostname = "zanos";
    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
    netif->output = etharp_output;
    netif->linkoutput = low_level_output;
    netif->mtu = 1500;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

    /* MAC address is set in main/eth.c, but we should inform lwIP */
    // netif->hwaddr is filled outside
    
    return ERR_OK;
}
