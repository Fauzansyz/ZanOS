#include "uart.h"
#include "ramdisk.h"
#include "strings.h"
#include "delay.h"
#include "kernel_log.h"
#include <stdbool.h>
#include "heap.h"
#include "rtc.h"
#include "eth.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/etharp.h"
#include "netif/ethernet.h"
#include "lwip/dhcp.h"
#include "lwip/timeouts.h"
#include "lwip/debug.h"
#include "lwip/dns.h"
#include "http_client.h"

struct netif zanos_netif;
extern err_t ethernetif_init(struct netif *netif);
extern void ethernetif_input(struct netif *netif);

#define HEAP_START 0x20000000
#define HEAP_SIZE  (64 * 1024)

#define CMD_MAX 64
extern volatile bool eth_rx_flag;

static volatile bool dns_resolved = false;
static ip_addr_t resolved_ip;

static void dns_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg) {
    (void)callback_arg;
    if (ipaddr != NULL) {
        resolved_ip = *ipaddr;
    } else {
        ip_addr_set_zero(&resolved_ip);
    }
    dns_resolved = true;
}

void lwip_poll(void) {
    sys_check_timeouts();
    if (eth_rx_flag) {
        eth_rx_flag = false;
        ethernetif_input(&zanos_netif);
    }
}

// Fungsi getline untuk input dari UART (non-blocking untuk lwIP)
void getline(char *buf, int max) {
    int i = 0;

    while (1) {
        lwip_poll();

        if (uart_has_data()) {
            char c = uart_getc();

            // ENTER (CR or LF)
            if (c == '\r' || c == '\n') {
                uart_println(""); 
                buf[i] = '\0';
                return;
            }

            // Backspace
            if (c == 127 || c == '\b') {
                if (i > 0) {
                    i--;
                    uart_print("\b \b");
                }
                continue;
            }

            // Simpan karakter biasa
            if (i < max-1) {
                buf[i++] = c;
                uart_putc(c); // echo
            }
        }
    }
}

void main(void) {
    extern char _sheap;
    extern void interrupts_enable(void);
    extern void interrupts_disable(void);

    heap_init(&_sheap, 8 * 1024);
    uart_println("HEAP INIT DONE");
    uart_println("=== ZanOS v0.4 ===");
    diag_init();
    ramdisk_init();
    rtc_init();
    
    uint8_t mac_addr[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02};
    eth_init(mac_addr);
    
    /* Initialize lwIP stack with interrupts disabled to prevent races */
    interrupts_disable();
    lwip_init();

    /* Add our network interface */
    ip4_addr_t ipaddr, netmask, gw;
    ip4_addr_set_zero(&ipaddr);
    ip4_addr_set_zero(&netmask);
    ip4_addr_set_zero(&gw);

    netif_add(&zanos_netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, ethernet_input);
    netif_set_default(&zanos_netif);
    netif_set_up(&zanos_netif);
    
    /* Inform lwIP of our MAC address */
    zanos_netif.hwaddr_len = 6;
    memcpy(zanos_netif.hwaddr, mac_addr, 6);

    /* Start DHCP to get an IP automatically */
    dhcp_start(&zanos_netif);
    
    interrupts_enable();
    
    uart_println("[LWIP] lwIP Stack Initialized & DHCP Started.");
    
    /* Configure SysTick for 10ms ticks (assuming 16MHz clock for LM3S) */
    (*(volatile uint32_t *)0xE000E014) = 160000 - 1; // Reload value
    (*(volatile uint32_t *)0xE000E018) = 0;          // Current value
    (*(volatile uint32_t *)0xE000E010) = 0x07;       // Enable, Interrupt, Clock Source
    
    interrupts_enable(); // Aktifkan interupsi hardware
    uart_println("Interrupts enabled.");
    
    void *a = kmalloc(100);
    void *b = kmalloc(28);
    (void)a; (void)b;

    // Demo RAM disk
    char buffer[128];
    ramdisk_create("file1");
    ramdisk_write("file1", "Hello FauzanOS!");
    ramdisk_read("file1", buffer);
    uart_print("Read from file1: ");
    uart_println(buffer);

    char cmd[CMD_MAX];

    while (true) {
        uart_print("> ");
        getline(cmd, CMD_MAX); // nanti input dari UART

        if (!cmd[0]) continue;

        // perintah shell minimal
        if (strcmp(cmd, "help") == 0) {
            uart_println("Available commands:");
            uart_println("  help  - show this message");
            uart_println("  hello - greet Fauzan");
            uart_println("  info  - kernel info");
            uart_println("  clear - clear screen (simulated)");
            uart_println("  ls    - list RAM disk files");
            uart_println("  cat   - read file contents");
            uart_println("  time  - show current RTC time");
            uart_println("  host  - resolve domain name");
            uart_println("  curl  - fetch HTTP (e.g., curl example.com)");
        } 
        else if (strcmp(cmd, "hello") == 0) {
            uart_println("Hello Fauzan!");
        } 
        else if (strncmp(cmd, "curl ", 5) == 0) {
            char *hostname = cmd + 5;
            http_get(hostname, "/");
            while (http_is_busy()) {
                lwip_poll();
            }
        }
        else if (strcmp(cmd, "info") == 0) {
            uart_println("FauzanOS v0.4 - Minimal ARM64 kernel with RAM disk");
        } 
        else if (strcmp(cmd, "clear") == 0) {
            uart_println("\n\n\n\n\n"); // simulasi clear
        } 
        else if (strcmp(cmd, "ls") == 0) {
            ramdisk_ls(); // list files di RAM disk
        } 
        else if (strcmp(cmd, "ifconfig") == 0) {
            uart_print("Interface: ");
            uart_putc(zanos_netif.name[0]);
            uart_putc(zanos_netif.name[1]);
            uart_println("");
            
            uart_print("  IP:      ");
            uart_println(ip4addr_ntoa(netif_ip4_addr(&zanos_netif)));
            uart_print("  Mask:    ");
            uart_println(ip4addr_ntoa(netif_ip4_netmask(&zanos_netif)));
            uart_print("  Gateway: ");
            uart_println(ip4addr_ntoa(netif_ip4_gw(&zanos_netif)));
            
            if (dhcp_supplied_address(&zanos_netif)) {
                uart_println("  Status:  IP assigned via DHCP");
            } else {
                uart_println("  Status:  Waiting for DHCP...");
            }
        }
        else if (strncmp(cmd, "host ", 5) == 0) {
            char *domain = cmd + 5;
            uart_print("Resolving ");
            uart_println(domain);
            
            ip_addr_t dns_ip;
            err_t err = dns_gethostbyname(domain, &dns_ip, dns_callback, NULL);
            if (err == ERR_OK) {
                uart_print("IP: ");
                uart_println(ipaddr_ntoa(&dns_ip));
            } else if (err == ERR_INPROGRESS) {
                dns_resolved = false;
                int timeout = 0;
                while (!dns_resolved && timeout < 2000000) {
                    lwip_poll();
                    timeout++;
                }
                if (dns_resolved) {
                    if (!ip_addr_isany(&resolved_ip)) {
                        uart_print("IP: ");
                        uart_println(ipaddr_ntoa(&resolved_ip));
                    } else {
                        uart_println("Failed to resolve.");
                    }
                } else {
                    uart_println("DNS resolution timed out.");
                }
            } else {
                uart_println("DNS error.");
            }
        }
        else if (strncmp(cmd, "cat ", 4) == 0) {
            char *filename = cmd + 4;
            if (ramdisk_read(filename, buffer) == 0) {
                uart_println(buffer);
            } else {
                uart_println("File not found");
            }
        } else if (strncmp(cmd, "diagnostic", 4) == 0 ) {
          diag_print();
        }
        else if (strcmp(cmd, "time") == 0) {
            struct rtc_time tm;
            rtc_get_time(&tm);
            uart_print("Current time: ");
            if (tm.tm_hour < 10) uart_putc('0');
            uart_print_int(tm.tm_hour);
            uart_print(":");
            if (tm.tm_min < 10) uart_putc('0');
            uart_print_int(tm.tm_min);
            uart_print(":");
            if (tm.tm_sec < 10) uart_putc('0');
            uart_print_int(tm.tm_sec);
            uart_println("");
        }
        else {
            uart_println("Unknown command");
        }
    }
}

