/**
 * @file rtc.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.1
 * @date 2019-11-18
 * 
 * @copyright Copyright Keeton Feavel et al (c) 2019
 * 
 */

#include <devices/rtc/rtc.hpp>
#include <arch/i386/isr.hpp>

void px_rtc_callback(registers_t regs);
// Current values from RTC
uint8_t second; // Current UTC second
uint8_t minute; // Current UTC minute
uint8_t hour;   // Current UTC hour
uint8_t day;    // Current UTC day (not reliable)
uint8_t month;  // Current UTC month
uint32_t year;  // Current UTC year

void px_rtc_init() {
    px_print_debug("Initializing RTC", Info);
    // Initializer
    px_write_byte(RTC_CMOS_PORT, 0x8A);
    px_write_byte(RTC_DATA_PORT, 0x20);
    // Enable IRQ 8 - Make sure interrupts are disabled beforehand
    px_write_byte(RTC_CMOS_PORT, 0x8B);
    char prev = px_read_byte(RTC_DATA_PORT);
    px_write_byte(RTC_CMOS_PORT, 0x8B);
    px_write_byte(RTC_DATA_PORT, (prev | 0x40));
    // Register our callback function with IRQ 8
    px_register_interrupt_handler(IRQ8, px_rtc_callback);
}

void px_rtc_callback(registers_t regs) {
    kprint("RTC update here.\n");
}

int px_rtc_get_update_in_progress() {
    px_write_byte(RTC_CMOS_PORT, 0x0A);
    return (px_read_byte(RTC_DATA_PORT) & 0x80);
}

uint8_t px_rtc_get_register(int reg) {
    px_write_byte(RTC_CMOS_PORT, reg);
    return px_read_byte(RTC_DATA_PORT);
}

/*
    Big thanks to the OSDev Wiki for providing this function. It saved me a lot of time an energy trying to
    debug and convert and do whatever to get RTC to work. I don't know who it was who wrote this code originally,
    but thanks a lot. Time in programming is the worst.
    Note: This uses the "read registers until you get the same values twice in a row" technique
    to avoid getting dodgy/inconsistent values due to RTC updates
*/
void px_read_rtc() {
    // Previous values from RTC
    // Used as a cache to check if we should update
    uint8_t century, last_second, last_minute, 
            last_hour, last_day, last_month,
            last_year, last_century, registerB;
    // Make sure an update isn't in progress
    while (px_rtc_get_update_in_progress());

    second = px_rtc_get_register(0x00);
    minute = px_rtc_get_register(0x02);
    hour = px_rtc_get_register(0x04);
    day = px_rtc_get_register(0x07);
    month = px_rtc_get_register(0x08);
    year = px_rtc_get_register(0x09);

    if (RTC_CURRENT_CENTURY != 0) {
        century = px_rtc_get_register(RTC_CURRENT_CENTURY);
    }
 
    while  ((last_second != second) || (last_minute != minute) || 
            (last_hour != hour)     || (last_day != day)       || 
            (last_month != month)   || (last_year != year)     ||
            (last_century != century))
    {
        last_second = second;
        last_minute = minute;
        last_hour = hour;
        last_day = day;
        last_month = month;
        last_year = year;
        last_century = century;

        // Make sure an update isn't in progress
        while (px_rtc_get_update_in_progress());

        second = px_rtc_get_register(0x00);
        minute = px_rtc_get_register(0x02);
        hour = px_rtc_get_register(0x04);
        day = px_rtc_get_register(0x07);
        month = px_rtc_get_register(0x08);
        year = px_rtc_get_register(0x09);
        if (RTC_CURRENT_CENTURY != 0) {
            century = px_rtc_get_register(RTC_CURRENT_CENTURY);
        }
    }
 
    registerB = px_rtc_get_register(0x0B);

    // Convert BCD to binary values if necessary
    if (!(registerB & 0x04)) {
        second = (second & 0x0F) + ((second / 16) * 10);
        minute = (minute & 0x0F) + ((minute / 16) * 10);
        hour = ( (hour & 0x0F) + (((hour & 0x70) / 16) * 10) ) | (hour & 0x80);
        day = (day & 0x0F) + ((day / 16) * 10);
        month = (month & 0x0F) + ((month / 16) * 10);
        year = (year & 0x0F) + ((year / 16) * 10);

        if (RTC_CURRENT_CENTURY != 0) {
            century = (century & 0x0F) + ((century / 16) * 10);
        }
    }

    // Convert 12 hour clock to 24 hour clock if necessary
    if (!(registerB & 0x02) && (hour & 0x80)) {
        hour = ((hour & 0x7F) + 12) % 24;
    }

    // Calculate the full (4-digit) year
    if(RTC_CURRENT_CENTURY != 0) {
        year += century * 100;
    } else {
        year += (RTC_CURRENT_YEAR / 100) * 100;
        if(year < RTC_CURRENT_YEAR) year += 100;
    }
}

void px_rtc_print() {
    px_read_rtc();
    char hourStr[3];
    char minuteStr[3];
    char dayStr[3];
    char monthStr[3];
    itoa(hour, hourStr);
    itoa(minute, minuteStr);
    itoa(day, dayStr);
    itoa(month, monthStr);
    kprint("\nToday's Date: ");
    kprint(monthStr);
    kprint("/");
    kprint(dayStr);
    kprint(" - UTC: ");
    kprint(hourStr);
    kprint(":");
    kprint(minuteStr);
    kprint("\n\n");
}