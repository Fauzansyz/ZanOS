#include "uart.h"
#include "ramdisk.h"
#include "strings.h"
#include "delay.h"
#include "kernel_log.h"
#include <stdbool.h>
#include "heap.h"
#include "rtc.h"

#define HEAP_START 0x20000000
#define HEAP_SIZE  (64 * 1024)

#define CMD_MAX 64
// Fungsi getline untuk input dari UART (saat ini dummy, bisa diupdate nanti)
void getline(char *buf, int max) {
    int i = 0;

    while (1) {
        char c = uart_getc();  // <-- HARUS BLOCKING DI SINI

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

void main(void) {
    extern char _end;

    heap_init(&_end, 64 * 1024);
    uart_println("=== ZanOS v0.4 ===");
    diag_init();
    ramdisk_init();
    rtc_init();
    
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
        } 
        else if (strcmp(cmd, "hello") == 0) {
            uart_println("Hello Fauzan!");
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

