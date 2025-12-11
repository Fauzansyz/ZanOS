#include "uart.h"
#include "ramdisk.h"
#include "strings.h"
#include "delay.h"
#include <stdbool.h>

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
    uart_println("=== FauzanOS v0.4 Shell ===");
    ramdisk_init();

    // Demo RAM disk
    char buffer[128];
    ramdisk_create("file1");
    ramdisk_write("file1", "Hello FauzanOS!");
    ramdisk_read("file1", buffer);
    uart_print("Read from file1: ");
    uart_println(buffer);

    uart_println("Demo complete. Entering shell...");

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
        } 
        else {
            uart_println("Unknown command");
        }
    }
}