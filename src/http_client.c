#include "http_client.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"
#include "uart.h"
#include "strings.h"

static struct tcp_pcb *http_pcb = NULL;
static bool http_busy = false;
static char request_buf[256];

static void http_close_conn(struct tcp_pcb *tpcb) {
    tcp_arg(tpcb, NULL);
    tcp_sent(tpcb, NULL);
    tcp_recv(tpcb, NULL);
    tcp_err(tpcb, NULL);
    tcp_poll(tpcb, NULL, 0);
    tcp_close(tpcb);
    http_pcb = NULL;
    http_busy = false;
    uart_println("\n[HTTP] Connection closed.");
}

static err_t http_recv_cb(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    (void)arg;
    if (p == NULL) {
        // Server menutup koneksi
        http_close_conn(tpcb);
        return ERR_OK;
    }

    if (err != ERR_OK) {
        if (p != NULL) pbuf_free(p);
        return err;
    }

    // Print data yang diterima
    for (struct pbuf *q = p; q != NULL; q = q->next) {
        char *data = (char *)q->payload;
        for (int i = 0; i < q->len; i++) {
            uart_putc(data[i]);
        }
    }

    // Beritahu lwIP bahwa kita sudah memproses data ini
    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);

    return ERR_OK;
}

static void http_err_cb(void *arg, err_t err) {
    (void)arg;
    uart_print("\n[HTTP] Error: ");
    uart_print_int(err);
    uart_println("");
    http_pcb = NULL;
    http_busy = false;
}

static err_t http_connected_cb(void *arg, struct tcp_pcb *tpcb, err_t err) {
    (void)arg;
    if (err != ERR_OK) {
        uart_println("\n[HTTP] Connection failed.");
        http_close_conn(tpcb);
        return err;
    }

    uart_println("[HTTP] Connected. Sending request...");

    // Setup callbacks untuk menerima data
    tcp_recv(tpcb, http_recv_cb);
    tcp_err(tpcb, http_err_cb);

    // Kirim request GET
    err_t write_err = tcp_write(tpcb, request_buf, strlen(request_buf), TCP_WRITE_FLAG_COPY);
    if (write_err == ERR_OK) {
        tcp_output(tpcb); // Paksa kirim sekarang
    } else {
        uart_println("[HTTP] Failed to write request.");
        http_close_conn(tpcb);
    }

    return ERR_OK;
}

static void http_dns_cb(const char *name, const ip_addr_t *ipaddr, void *callback_arg) {
    (void)name;
    (void)callback_arg;

    if (ipaddr == NULL) {
        uart_println("[HTTP] DNS resolution failed.");
        http_busy = false;
        return;
    }

    uart_print("[HTTP] Resolved IP: ");
    uart_println(ipaddr_ntoa(ipaddr));

    // Buat PCB TCP baru
    http_pcb = tcp_new();
    if (http_pcb == NULL) {
        uart_println("[HTTP] Failed to create TCP PCB.");
        http_busy = false;
        return;
    }

    tcp_arg(http_pcb, NULL);
    tcp_err(http_pcb, http_err_cb);

    // Connect ke port 80 (HTTP)
    uart_println("[HTTP] Connecting to server...");
    err_t err = tcp_connect(http_pcb, ipaddr, 80, http_connected_cb);
    if (err != ERR_OK) {
        uart_println("[HTTP] tcp_connect error.");
        http_close_conn(http_pcb);
    }
}

void http_get(const char *hostname, const char *path) {
    if (http_busy) {
        uart_println("[HTTP] Client is already busy.");
        return;
    }

    http_busy = true;

    // Siapkan HTTP GET Request sederhana
    // Catatan: Tidak semua server web modern mendukung HTTP/1.0 sederhana tanpa header Host,
    // tapi untuk keperluan dasar ini kita sertakan Host.
    strcpy(request_buf, "GET ");
    strcpy(request_buf + strlen(request_buf), path);
    strcpy(request_buf + strlen(request_buf), " HTTP/1.1\r\nHost: ");
    strcpy(request_buf + strlen(request_buf), hostname);
    strcpy(request_buf + strlen(request_buf), "\r\nConnection: close\r\n\r\n");

    uart_print("[HTTP] Resolving ");
    uart_println(hostname);

    ip_addr_t server_ip;
    err_t err = dns_gethostbyname(hostname, &server_ip, http_dns_cb, NULL);

    if (err == ERR_OK) {
        // DNS sudah ada di cache
        http_dns_cb(hostname, &server_ip, NULL);
    } else if (err == ERR_INPROGRESS) {
        // Tunggu callback
    } else {
        uart_println("[HTTP] DNS error.");
        http_busy = false;
    }
}

bool http_is_busy(void) {
    return http_busy;
}
