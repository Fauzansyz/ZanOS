#include "heap.h"
#include "uart.h"

static unsigned char *heap_ptr;
static unsigned char *heap_end;
static unsigned int used = 0;

void heap_init(void *start, unsigned int size) {
    heap_ptr = (unsigned char *)start;
    heap_end = heap_ptr + size;
    used = 0;

    uart_println("HEAP INIT");
}

void *kmalloc(unsigned int size) {
    if (heap_ptr + size >= heap_end)
        return 0;

    void *p = heap_ptr;
    heap_ptr += size;
    used += size;

    uart_println("KMALLOC");
    return p;
}

unsigned int heap_used(void) {
    return used;
}