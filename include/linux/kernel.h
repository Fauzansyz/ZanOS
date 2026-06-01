#ifndef _LINUX_KERNEL_H
#define _LINUX_KERNEL_H

// Hubungkan printk Linux ke fungsi cetak teks UART milik kernel lu
extern void uart_puts(char* s);
#define printk(fmt, ...) uart_puts(fmt)
//
// // Fungsi pembantu konversi bilangan BCD (Binary Coded Decimal) ke biner biasa
// // Biasanya hardware RTC jadul nyimpen waktu format BCD
#define bcd2bin(x) (((x) & 0x0f) + ((x) >> 4) * 10)
#define bin2bcd(x) ((((x) / 10) << 4) + (x) % 10)
//
 #endif
// 
