#ifndef _LINUX_RTC_H
#define _LINUX_RTC_H

// Struktur data standar Linux untuk menampung waktu
struct rtc_time {
    int tm_sec;   // Detik (0-59)
    int tm_min;   // Menit (0-59)
    int tm_hour;  // Jam (0-23)
    int tm_mday;  // Hari/Tanggal (1-31)
    int tm_mon;   // Bulan (0-11, Januari = 0)
    int tm_year;  // Tahun (sejak tahun 1900)
    int tm_wday;  // Hari dalam seminggu (0-6, Minggu = 0)
    int tm_yday;  // Hari dalam setahun (0-365)
    int tm_isdst; // Fitur Daylight Saving Time
};

#endif

