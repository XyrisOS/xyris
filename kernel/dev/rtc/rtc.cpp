/**
 * @file rtc.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @author Michel (JMallone) Gomes (michels@utfpr.edu.br)
 * @brief
 * @version 0.4
 * @date 2021-07-25
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 * References:
 *         https://github.com/limine-bootloader/limine/blob/trunk/stage23/lib/time.c
 *         https://en.wikipedia.org/wiki/Julian_day
 */
#include <arch/i386/ports.hpp>
#include <arch/i386/isr.hpp>
#include <dev/rtc/rtc.hpp>
#include <dev/tty/tty.hpp>
#include <lib/stdio.hpp>
#include <lib/string.hpp>

#define RTC_CMOS_PORT 0x70
#define RTC_DATA_PORT 0x71
#define RTC_CURRENT_YEAR 2021 // Needs to be updated every year!
#define RTC_CURRENT_CENTURY 0 // Needs to be updated every century!

static void rtc_callback(registers_t* regs);
static bool rtc_get_update_in_progress();
static uint8_t rtc_get_register(uint8_t reg);
static void read_rtc();

// Current values from RTC
// These variables are way larger than they ever
// should be, but the compiler doesn't like our
// math with anything smaller, so we're going
// to live with it since memory is "cheap"
uint32_t rtc_second;  // Current UTC second
uint32_t rtc_minute;  // Current UTC minute
uint32_t rtc_hour;    // Current UTC hour
uint32_t rtc_day;     // Current UTC day (not reliable)
uint32_t rtc_month;   // Current UTC month
uint32_t rtc_year;    // Current UTC year
uint32_t rtc_century; // Current UTC century

void rtc_init()
{
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

static void rtc_callback(registers_t* regs)
{
    (void)regs;
    kprintf(DBG_INFO "RTC updated.\n");
}

static bool rtc_get_update_in_progress()
{
    writeByte(RTC_CMOS_PORT, 0x0A);
    return (readByte(RTC_DATA_PORT) & 0x80);
}

static uint8_t rtc_get_register(uint8_t reg)
{
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
static void read_rtc()
{
    // Previous values from RTC
    // Used as a cache to check if we should update
    uint32_t last_second = 0, last_minute = 0,
             last_hour = 0, last_day = 0,
             last_month = 0, last_year = 0,
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

    while ((last_second != rtc_second) || (last_minute != rtc_minute) ||
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
        rtc_second = (rtc_second & 0x0F) + ((rtc_second / 16) * 10);
        rtc_minute = (rtc_minute & 0x0F) + ((rtc_minute / 16) * 10);
        rtc_hour = ((rtc_hour & 0x0F) + (((rtc_hour & 0x70) / 16) * 10)) | (rtc_hour & 0x80);
        rtc_day = (rtc_day & 0x0F) + ((rtc_day / 16) * 10);
        rtc_month = (rtc_month & 0x0F) + ((rtc_month / 16) * 10);
        rtc_year = (rtc_year & 0x0F) + ((rtc_year / 16) * 10);

        if (RTC_CURRENT_CENTURY != 0) {
            rtc_century = (rtc_century & 0x0F) + ((rtc_century / 16) * 10);
        }
    }

    // Convert 12 hour clock to 24 hour clock if necessary
    if (!(registerB & 0x02) && (rtc_hour & 0x80)) {
        rtc_hour = ((rtc_hour & 0x7F) + 12) % 24;
    }

    // Calculate the full (4-digit) year
    if (RTC_CURRENT_CENTURY != 0) {
        rtc_year += rtc_century * 100;
    } else {
        rtc_year += (RTC_CURRENT_YEAR / 100) * 100;
        if (rtc_year < RTC_CURRENT_YEAR)
            rtc_year += 100;
    }
}

/**
 * JDN is time since noon Universal Time on January 1, 4713 BCE (on the Julian calendar)
 * Developed by Fliegel and van Flandern (1968), published in "Compact Computer Algorithms"
 *
 *           (153 * m) + 2                y     y     y
 * JDN = d + ------------- + (365 * y) + --- - --- + --- - 32045
 *                  5                     4    100   400
 *           <  Part #1  >  < Part #2 >  <   Part #3   > < Part #4 >
 *
 * Part #1: Calculates the number of days in the previous months (where March -> m = 0)
 * Part #2: Adds the number of days in all non-leap years
 * Part #3: Calculates and adds the number of leap years since the year -4800
 *          (which is 0 for y). Leap years occur on every 4th, 100th, and 400th
 *           year, which corresponds to each sub-step in this part)
 * Part #4: Ensure that the result will be 0 for January 1, 4713 BCE
 *         (This is the start of the Julian Calendar)
 */
static uint64_t getJulianDay(uint8_t days, uint8_t months, uint16_t years)
{
    int a = (14 - months) / 12;
    int y = years + 4800 - a;
    int m = months + 12 * a - 3;

    int jdn = days + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
    return jdn;
}

static uint64_t getUnixEpoch(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t days, uint8_t months, uint16_t years)
{
    uint64_t jdnNow = getJulianDay(days, months, years);
    uint64_t jdn1970 = getJulianDay(1, 1, 1970);
    uint64_t jdnDiff = jdnNow - jdn1970;
    return (jdnDiff * (60 * 60 * 24)) + hours * 3600 + minutes * 60 + seconds;
}

uint64_t getEpoch()
{
    read_rtc();
    return getUnixEpoch(rtc_second, rtc_minute, rtc_hour, rtc_day, rtc_month, rtc_year);
}
