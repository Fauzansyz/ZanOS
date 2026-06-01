#include "rtc.h"

#define HIB_BASE      0x40033000
#define HIB_RTCC      (*(volatile unsigned int *)(HIB_BASE + 0x000))
#define HIB_CTL       (*(volatile unsigned int *)(HIB_BASE + 0x010))

#define HIB_CTL_RTCEN (1 << 6)

void rtc_init(void) {
    // Di LM3S6965, RTC ada di modul Hibernation.
    // Kita perlu memastikan RTC diaktifkan.
    if (!(HIB_CTL & HIB_CTL_RTCEN)) {
        HIB_CTL |= HIB_CTL_RTCEN;
    }
}

unsigned int rtc_get_seconds(void) {
    return HIB_RTCC;
}

void rtc_get_time(struct rtc_time *tm) {
    unsigned int t = rtc_get_seconds();
    
    tm->tm_sec = t % 60;
    t /= 60;
    tm->tm_min = t % 60;
    t /= 60;
    tm->tm_hour = t % 24;
    t /= 24;

    // Untuk tanggal, ini sangat dasar (hanya hari sejak reset)
    // Implementasi penuh butuh epoch Unix atau kalender Gregorian
    tm->tm_mday = (t % 30) + 1;
    tm->tm_mon = (t / 30) % 12;
    tm->tm_year = (t / 365) + 100; // Misal 2000-an
}
