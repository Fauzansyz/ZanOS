#include "ramdisk.h"
#include "uart.h"
#include "strings.h"
#include "kernel_log.h"
#include "heap.h"

static File files[MAX_FILES];

void ramdisk_init(void) {
    for (int i = 0; i < MAX_FILES; i++)
        files[i].used = 0;
}

int ramdisk_create(const char *name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!files[i].used) {
            files[i].used = 1;
            strncpy(files[i].name, name, 31);
            files[i].name[31] = '\0';  // pastikan null-terminated
            files[i].size = 0;
            diag_file_created();
            return 0; // sukses
        }
    }
    return -1; // full
}

int ramdisk_write(const char *name, const char *data) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strcmp(files[i].name, name) == 0) {
            int j;
            for (j = 0; data[j] && j < MAX_FILE_SIZE - 1; j++)
                files[i].data[j] = data[j];
            files[i].data[j] = '\0'; // selalu null-terminated
            files[i].size = j;
            return 0;
        }
    }
    return -1;
}

int ramdisk_read(const char *name, char *buffer) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strcmp(files[i].name, name) == 0) {
            for (int j = 0; j < files[i].size; j++)
                buffer[j] = files[i].data[j];
            buffer[files[i].size] = '\0';  // pastikan null-terminated
            return 0;
        }
    }
    return -1;
}

void ramdisk_ls(void) {
    uart_println("Files in RAM Disk:");
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used) {
            uart_print("- ");
            uart_println(files[i].name);
        }
    }
}