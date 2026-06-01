#include "uart.h"
#include <stdarg.h>

#define UART0DR   (*(volatile unsigned int *)0x4000C000)
#define UART0FR   (*(volatile unsigned int *)0x4000C018)

char uart_getc(void) {
    // tunggu sampai ADA data (RXFE == 0)
    while (UART0FR & (1 << 4)) {
        // RXFE=1 → masih kosong → tunggu
    }

    return (char)(UART0DR & 0xFF);
}

int uart_has_data(void) {
    return !(UART0FR & (1 << 4));
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
void uart_print_int(int n) {
    char buf[16];
    int i = 0;

    if (n == 0) {
        uart_putc('0');
        return;
    }

    while (n > 0) {
        int digit = 0;
        while (n >= 10) {
            n -= 10;
            digit++;
        }
        buf[i++] = '0' + n;
        n = digit;
    }

    while (i--)
        uart_putc(buf[i]);
}

void uart_println(const char *s) {
    uart_print(s);
    uart_putc('\n');
}

static void uart_print_hex(unsigned int n) {
    const char *hex = "0123456789ABCDEF";
    for (int i = 28; i >= 0; i -= 4) {
        uart_putc(hex[(n >> i) & 0xF]);
    }
}

void uart_printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            if (*fmt == 's') {
                uart_print(va_arg(args, char *));
            } else if (*fmt == 'd') {
                uart_print_int(va_arg(args, int));
            } else if (*fmt == 'x' || *fmt == 'p' || *fmt == 'X') {
                uart_print_hex(va_arg(args, unsigned int));
            } else if (*fmt == '%') {
                uart_putc('%');
            }
        } else {
            uart_putc(*fmt);
        }
        fmt++;
    }
    va_end(args);
}