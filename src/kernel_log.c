#include "uart.h"
#include "kernel_log.h"
#include "heap.h"
#include "rtc.h"

static int uptime_sec = 0;
static int file_count = 0;

void diag_init(void) {
    uptime_sec = 0;
    file_count = 0;
}

void diag_tick(void) {
    uptime_sec++;
}

void diag_file_created(void) {
    file_count++;
}

void diag_print(void) {
    uart_println("=== System Diagnosis ===");
    uart_print("Uptime :");
    
    // Gunakan RTC jika tersedia untuk uptime yang lebih akurat
    unsigned int current_uptime = rtc_get_seconds();
    if (current_uptime == 0) current_uptime = uptime_sec;

    uart_print_int(current_uptime);
    uart_print("s");
    
    uart_print("   ");
    
    uart_print("Files  :");
    uart_print_int(file_count);
    uart_print("   ");

    uart_print("Heap used : ");
    uart_print_int(heap_used());
    uart_println(" bytes");

    uart_println("Kernel: ZanOS v0.4");
}
