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

void px_rtc_callback(registers_t *regs);
int px_rtc_get_update_in_progress();
uint8_t px_rtc_get_register(int reg);
void px_read_rtc();

// Current values from RTC
uint8_t px_rtc_second;      // Current UTC second
uint8_t px_rtc_minute;      // Current UTC minute
uint8_t px_rtc_hour;        // Current UTC hour
uint8_t px_rtc_day;         // Current UTC day (not reliable)
uint8_t px_rtc_month;       // Current UTC month
uint32_t px_rtc_year;       // Current UTC year
uint32_t px_rtc_century;    // Current UTC century

void px_rtc_init() {
    px_kprintf(DBG_INFO "Initializing RTC...\n");
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

void px_rtc_callback(registers_t *regs) {
    (void)regs;
    px_kprintf(DBG_INFO "RTC updated.\n");
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
    uint8_t last_second = 0, last_minute = 0,
            last_hour = 0, last_day = 0,
            last_month = 0, last_year = 0,
            last_century = 0, registerB = 0;
    // Make sure an update isn't in progress
    while (px_rtc_get_update_in_progress());

    px_rtc_second = px_rtc_get_register(0x00);
    px_rtc_minute = px_rtc_get_register(0x02);
    px_rtc_hour = px_rtc_get_register(0x04);
    px_rtc_day = px_rtc_get_register(0x07);
    px_rtc_month = px_rtc_get_register(0x08);
    px_rtc_year = px_rtc_get_register(0x09);
    px_rtc_century = px_rtc_get_register(RTC_CURRENT_CENTURY);
 
    while  ((last_second != px_rtc_second) || (last_minute != px_rtc_minute) || 
            (last_hour != px_rtc_hour)     || (last_day != px_rtc_day)       || 
            (last_month != px_rtc_month)   || (last_year != px_rtc_year)     ||
            (last_century != px_rtc_century))
    {
        last_second = px_rtc_second;
        last_minute = px_rtc_minute;
        last_hour = px_rtc_hour;
        last_day = px_rtc_day;
        last_month = px_rtc_month;
        last_year = px_rtc_year;
        last_century = px_rtc_century;

        // Make sure an update isn't in progress
        while (px_rtc_get_update_in_progress());

        px_rtc_second = px_rtc_get_register(0x00);
        px_rtc_minute = px_rtc_get_register(0x02);
        px_rtc_hour = px_rtc_get_register(0x04);
        px_rtc_day = px_rtc_get_register(0x07);
        px_rtc_month = px_rtc_get_register(0x08);
        px_rtc_year = px_rtc_get_register(0x09);
        if (RTC_CURRENT_CENTURY != 0) {
            px_rtc_century = px_rtc_get_register(RTC_CURRENT_CENTURY);
        }
    }
 
    registerB = px_rtc_get_register(0x0B);

    // Convert BCD to binary values if necessary
    if (!(registerB & 0x04)) {
        px_rtc_second = (px_rtc_second & 0x0F) + ((px_rtc_second / 16) * 10);
        px_rtc_minute = (px_rtc_minute & 0x0F) + ((px_rtc_minute / 16) * 10);
        px_rtc_hour = ( (px_rtc_hour & 0x0F) + (((px_rtc_hour & 0x70) / 16) * 10) ) | (px_rtc_hour & 0x80);
        px_rtc_day = (px_rtc_day & 0x0F) + ((px_rtc_day / 16) * 10);
        px_rtc_month = (px_rtc_month & 0x0F) + ((px_rtc_month / 16) * 10);
        px_rtc_year = (px_rtc_year & 0x0F) + ((px_rtc_year / 16) * 10);

        if (RTC_CURRENT_CENTURY != 0) {
            px_rtc_century = (px_rtc_century & 0x0F) + ((px_rtc_century / 16) * 10);
        }
    }

    // Convert 12 hour clock to 24 hour clock if necessary
    if (!(registerB & 0x02) && (px_rtc_hour & 0x80)) {
        px_rtc_hour = ((px_rtc_hour & 0x7F) + 12) % 24;
    }

    // Calculate the full (4-digit) year
    if(RTC_CURRENT_CENTURY != 0) {
        px_rtc_year += px_rtc_century * 100;
    } else {
        px_rtc_year += (RTC_CURRENT_YEAR / 100) * 100;
        if(px_rtc_year < RTC_CURRENT_YEAR) px_rtc_year += 100;
    }
}

void px_rtc_print() {
    px_read_rtc();
    px_kprintf(
        DBG_INFO
        "UTC: %i/%i/%i %i:%i\n",
        px_rtc_month,
        px_rtc_day,
        px_rtc_year,
        px_rtc_hour,
        px_rtc_minute
    );
}
