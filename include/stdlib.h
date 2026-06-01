#ifndef STDLIB_H
#define STDLIB_H

#include "heap.h"
#include <stddef.h>

#define malloc(sz) kmalloc(sz)
#define free(p)    kfree(p)

#endif
