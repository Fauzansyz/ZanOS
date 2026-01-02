#ifndef HEAP_H
#define HEAP_H

void heap_init(void *start, unsigned int size);
void *kmalloc(unsigned int size);

unsigned int heap_used(void);

#endif