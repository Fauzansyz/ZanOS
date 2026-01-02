#ifndef RAMDISK_H
#define RAMDISK_H

#include <stdint.h>

#define MAX_FILES 16
#define MAX_FILE_SIZE 256


typedef struct kernel_status {
    int uptime;
    int total_files;
    int memory_used;
};

typedef struct {
    char name[32];
    uint8_t data[MAX_FILE_SIZE];
    uint32_t size;
    uint8_t used;
} File;

void ramdisk_init(void);
int ramdisk_create(const char *name);
int ramdisk_write(const char *name, const char *data);
int ramdisk_read(const char *name, char *buffer);
void ramdisk_ls(void);
void *kmalloc(unsigned int size);

#endif