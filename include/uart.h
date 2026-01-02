#ifndef UART_H
#define UART_H

void uart_putc(char c);
void uart_print(const char *s);
void uart_println(const char *s);
void uart_print_int(int n);

char uart_getc(void);
#endif