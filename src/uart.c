#include "uart.h"

#define UART0DR   (*(volatile unsigned int *)0x4000C000)
#define UART0FR   (*(volatile unsigned int *)0x4000C018)

char uart_getc() {
    char c;
    do {
        c = UART0DR & 0xFF; // baca data dari UART
    } while (UART0FR & (1 << 4)); // tunggu data ready
    return c;
}

void uart_print(const char *s) {
    while (*s) {
        while (UART0FR & (1 << 5));
        UART0DR = (unsigned int)(*s);
        s++;
    }
}

void uart_putc(char c) {
    while (UART0FR & (1 << 5)); // tunggu FIFO kosong
    UART0DR = c;
}

void uart_println(const char *s) {
    uart_print(s);
    uart_putc('\n');
}