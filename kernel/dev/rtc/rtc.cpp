/**
 * @file rtc.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2019-11-18
 *
 * @copyright Copyright the Panix Contributors (c) 2019
 *
 */

#include <arch/arch.hpp>
#include <dev/tty/tty.hpp>
#include <dev/rtc/rtc.hpp>
#include <lib/string.hpp>
#include <lib/stdio.hpp>

void rtc_callback(registers_t *regs);
bool rtc_get_update_in_progress();
uint8_t rtc_get_register(uint8_t reg);
void read_rtc();

// Current values from RTC
// These variables are way larger than they ever
// should be, but the compiler doesn't like our
// math with anything smaller, so we're going
// to live with it since memory is "cheap"
uint32_t rtc_second;      // Current UTC second
uint32_t rtc_minute;      // Current UTC minute
uint32_t rtc_hour;        // Current UTC hour
uint32_t rtc_day;         // Current UTC day (not reliable)
uint32_t rtc_month;       // Current UTC month
uint32_t rtc_year;        // Current UTC year
uint32_t rtc_century;     // Current UTC century

void rtc_init() {
    kprintf(DBG_INFO "Initializing RTC...\n");
    // Initializer
    writeByte(RTC_CMOS_PORT, 0x8A);
    writeByte(RTC_DATA_PORT, 0x20);
    // Enable IRQ 8 - Make sure interrupts are disabled beforehand
    writeByte(RTC_CMOS_PORT, 0x8B);
    char prev = readByte(RTC_DATA_PORT);
    writeByte(RTC_CMOS_PORT, 0x8B);
    writeByte(RTC_DATA_PORT, (prev | 0x40));
    // Register our callback function with IRQ 8
    register_interrupt_handler(IRQ8, rtc_callback);
}

void rtc_callback(registers_t *regs) {
    (void)regs;
    kprintf(DBG_INFO "RTC updated.\n");
}

bool rtc_get_update_in_progress() {
    writeByte(RTC_CMOS_PORT, 0x0A);
    return (readByte(RTC_DATA_PORT) & 0x80);
}

uint8_t rtc_get_register(uint8_t reg) {
    writeByte(RTC_CMOS_PORT, reg);
    return readByte(RTC_DATA_PORT);
}

/*
    Big thanks to the OSDev Wiki for providing this function. It saved me a lot of time an energy trying to
    debug and convert and do whatever to get RTC to work. I don't know who it was who wrote this code originally,
    but thanks a lot. Time in programming is the worst.
    Note: This uses the "read registers until you get the same values twice in a row" technique
    to avoid getting dodgy/inconsistent values due to RTC updates
*/
void read_rtc() {
    // Previous values from RTC
    // Used as a cache to check if we should update
    uint32_t last_second = 0, last_minute = 0,
            last_hour = 0, last_day = 0,
            last_month = 0,  last_year = 0,
            last_century = 0, registerB = 0;
    // Make sure an update isn't in progress
    while (rtc_get_update_in_progress());

    rtc_second = rtc_get_register(0x00);
    rtc_minute = rtc_get_register(0x02);
    rtc_hour = rtc_get_register(0x04);
    rtc_day = rtc_get_register(0x07);
    rtc_month = rtc_get_register(0x08);
    rtc_year = rtc_get_register(0x09);
    rtc_century = rtc_get_register(RTC_CURRENT_CENTURY);

    while  ((last_second != rtc_second) || (last_minute != rtc_minute) ||
            (last_hour != rtc_hour)     || (last_day != rtc_day)       ||
            (last_month != rtc_month)   || (last_year != rtc_year)     ||
            (last_century != rtc_century))
    {
        last_second = rtc_second;
        last_minute = rtc_minute;
        last_hour = rtc_hour;
        last_day = rtc_day;
        last_month = rtc_month;
        last_year = rtc_year;
        last_century = rtc_century;

        // Make sure an update isn't in progress
        while (rtc_get_update_in_progress());

        rtc_second = rtc_get_register(0x00);
        rtc_minute = rtc_get_register(0x02);
        rtc_hour = rtc_get_register(0x04);
        rtc_day = rtc_get_register(0x07);
        rtc_month = rtc_get_register(0x08);
        rtc_year = rtc_get_register(0x09);
        if (RTC_CURRENT_CENTURY != 0) {
            rtc_century = rtc_get_register(RTC_CURRENT_CENTURY);
        }
    }

    registerB = rtc_get_register(0x0B);

    // Convert BCD to binary values if necessary
    if (!(registerB & 0x04)) {
        rtc_second   = (rtc_second & 0x0F) + ((rtc_second / 16) * 10);
        rtc_minute   = (rtc_minute & 0x0F) + ((rtc_minute / 16) * 10);
        rtc_hour     = ((rtc_hour & 0x0F) + (((rtc_hour & 0x70) / 16) * 10)) | (rtc_hour & 0x80);
        rtc_day      = (rtc_day & 0x0F) + ((rtc_day / 16) * 10);
        rtc_month    = (rtc_month & 0x0F) + ((rtc_month / 16) * 10);
        rtc_year     = (rtc_year & 0x0F) + ((rtc_year / 16) * 10);

        if (RTC_CURRENT_CENTURY != 0) {
            rtc_century = (rtc_century & 0x0F) + ((rtc_century / 16) * 10);
        }
    }

    // Convert 12 hour clock to 24 hour clock if necessary
    if (!(registerB & 0x02) && (rtc_hour & 0x80)) {
        rtc_hour = ((rtc_hour & 0x7F) + 12) % 24;
    }

    // Calculate the full (4-digit) year
    if(RTC_CURRENT_CENTURY != 0) {
        rtc_year += rtc_century * 100;
    } else {
        rtc_year += (RTC_CURRENT_YEAR / 100) * 100;
        if(rtc_year < RTC_CURRENT_YEAR) rtc_year += 100;
    }
}

void rtc_print() {
    read_rtc();
    kprintf(
        DBG_INFO
        "UTC: %i/%i/%i %i:%i\n",
        rtc_month,
        rtc_day,
        rtc_year,
        rtc_hour,
        rtc_minute
    );
}
