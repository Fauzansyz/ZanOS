#include "uart.h"
#include "kernel_log.h"

static int uptime_sec = 0;
static int file_count = 0;
static int mem_used = 0;

void diag_init(void) {
    uptime_sec = 0;
    file_count = 0;
    mem_used = 0;
}

void diag_tick(void) {
    uptime_sec++;
}

void diag_file_created(void) {
    file_count++;
}

void diag_mem_used(int bytes) {
    mem_used += bytes;
}

void diag_print(void) {
    uart_println("=== System Diagnosis ===");
    uart_print("Uptime :");
    uart_print_int(uptime_sec);
    uart_print("s");
    
    uart_print("   ");
    
    uart_print("Files  :");
    uart_print_int(file_count);
    uart_print("   ");

    uart_print("Heap used : ");
    uart_print_int(mem_used);
    uart_println(" bytes");

    uart_println("Kernel: FauzanOS v0.5");
}
