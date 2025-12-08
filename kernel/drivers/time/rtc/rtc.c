#include "rtc.h"
#include "drivers/io/io.h"

#define CMOS_INDEX 0x70
#define CMOS_DATA  0x71

static inline uint8_t cmos_read(uint8_t reg) {
    outb(CMOS_INDEX, reg);
    return inb(CMOS_DATA);
}

static inline int rtc_is_updating(void) {
    outb(CMOS_INDEX, 0x0A);
    return inb(CMOS_DATA) & 0x80;
}

static inline uint8_t bcd_to_bin(uint8_t b) {
    return (b & 0x0F) + ((b >> 4) * 10);
}

void rtc_read(rtc_time_t* out) {
    uint8_t sec, min, hour, day, month, year;
    uint8_t regB;

    while (rtc_is_updating()) {}

    sec   = cmos_read(0x00);
    min   = cmos_read(0x02);
    hour  = cmos_read(0x04);
    day   = cmos_read(0x07);
    month = cmos_read(0x08);
    year  = cmos_read(0x09);

    regB = cmos_read(0x0B);

    if (!(regB & 0x04)) {
        sec   = bcd_to_bin(sec);
        min   = bcd_to_bin(min);
        hour  = bcd_to_bin(hour);
        day   = bcd_to_bin(day);
        month = bcd_to_bin(month);
        year  = bcd_to_bin(year);
    }

    uint16_t full_year = 2000 + year;
    if (full_year < 2024) full_year += 100; 
    out->second = sec;
    out->minute = min;
    out->hour   = hour;
    out->day    = day;
    out->month  = month;
    out->year   = full_year;
}

