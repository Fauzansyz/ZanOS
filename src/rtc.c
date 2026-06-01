#include "rtc.h"

// GPTM Timer0 Registers
#define SYSCTL_RCGC1_R          (*((volatile unsigned int *)0x400FE104))
#define TIMER0_CFG_R            (*((volatile unsigned int *)0x40030000))
#define TIMER0_TAMR_R           (*((volatile unsigned int *)0x40030004))
#define TIMER0_CTL_R            (*((volatile unsigned int *)0x4003000C))
#define TIMER0_TAILR_R          (*((volatile unsigned int *)0x40030028))
#define TIMER0_TAV_R            (*((volatile unsigned int *)0x40030050))

static unsigned int start_time_seconds = 0;

void rtc_init(void) {
    // Karena RTC Hibernation tidak jalan di QEMU, kita pakai GPTM Timer0
    // sebagai pengganti untuk menghitung detik sejak boot.
    
    // 1. Aktifkan clock untuk Timer0
    SYSCTL_RCGC1_R |= 0x00010000;
    
    // 2. Disable Timer0 untuk konfigurasi
    TIMER0_CTL_R &= ~0x1;
    
    // 3. Mode 32-bit (CFG = 0)
    TIMER0_CFG_R = 0x0;
    
    // 4. Periodic mode, Count down (TAMR = 2)
    TIMER0_TAMR_R = 0x2;
    
    // 5. Set reload value ke maksimal (0xFFFFFFFF)
    // LM3S6965 biasanya jalan di 50MHz, jadi 0xFFFFFFFF detik itu sangat lama.
    TIMER0_TAILR_R = 0xFFFFFFFF;
    
    // 6. Aktifkan Timer0
    TIMER0_CTL_R |= 0x1;

    start_time_seconds = 0;
}

unsigned int rtc_get_seconds(void) {
    // Hitung berapa detik telah berlalu berdasarkan nilai counter
    // Timer0 count down dari 0xFFFFFFFF.
    // Detik = (Awal - Sekarang) / Frekuensi (asumsi 50MHz di QEMU)
    unsigned int current_val = TIMER0_TAV_R;
    unsigned int elapsed_cycles = 0xFFFFFFFF - current_val;
    
    // 50.000.000 siklus = 1 detik
    return elapsed_cycles / 50000000;
}

void rtc_get_time(struct rtc_time *tm) {
    unsigned int t = rtc_get_seconds();
    
    tm->tm_sec = t % 60;
    t /= 60;
    tm->tm_min = t % 60;
    t /= 60;
    tm->tm_hour = t % 24;
    t /= 24;

    tm->tm_mday = (t % 30) + 1;
    tm->tm_mon = (t / 30) % 12;
    tm->tm_year = (t / 365) + 126; // 2026
}
