#ifndef _RTC_H
#define _RTC_H

#include "linux/rtc.h"

void rtc_init(void);
unsigned int rtc_get_seconds(void);
void rtc_get_time(struct rtc_time *tm);

#endif
