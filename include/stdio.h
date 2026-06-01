#ifndef STDIO_H
#define STDIO_H

#include "uart.h"
#include <stddef.h>

#define printf(fmt, ...) uart_println(fmt) // Very simplified for now

#endif
