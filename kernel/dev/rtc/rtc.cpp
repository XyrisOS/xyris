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
#include <arch/arch.hpp>
#include <dev/rtc/rtc.hpp>
#include <dev/tty/tty.hpp>
#include <lib/stdio.hpp>
#include <lib/string.hpp>

namespace rtc {

#define RTC_CMOS_PORT 0x70
#define RTC_DATA_PORT 0x71
#define RTC_CURRENT_YEAR 2021 // Needs to be updated every year!
#define RTC_CURRENT_CENTURY 0 // Needs to be updated every century!

static void callback(struct registers* regs);
static bool getUpdateInProgress();
static uint8_t getRegister(uint8_t reg);
static void read();

// Current values from RTC
// These variables are way larger than they ever
// should be, but the compiler doesn't like our
// math with anything smaller, so we're going
// to live with it since memory is "cheap"
uint32_t second;  // Current UTC second
uint32_t minute;  // Current UTC minute
uint32_t hour;    // Current UTC hour
uint32_t day;     // Current UTC day (not reliable)
uint32_t month;   // Current UTC month
uint32_t year;    // Current UTC year
uint32_t century; // Current UTC century

void init()
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
    register_interrupt_handler(IRQ8, callback);
}

static void callback(struct registers* regs)
{
    (void)regs;
    kprintf(DBG_INFO "RTC updated.\n");
}

static bool getUpdateInProgress()
{
    writeByte(RTC_CMOS_PORT, 0x0A);
    return (readByte(RTC_DATA_PORT) & 0x80);
}

static uint8_t getRegister(uint8_t reg)
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
static void read()
{
    // Previous values from RTC
    // Used as a cache to check if we should update
    uint32_t last_second = 0, last_minute = 0,
             last_hour = 0, last_day = 0,
             last_month = 0, last_year = 0,
             last_century = 0, registerB = 0;
    // Make sure an update isn't in progress
    while (getUpdateInProgress());

    second = getRegister(0x00);
    minute = getRegister(0x02);
    hour = getRegister(0x04);
    day = getRegister(0x07);
    month = getRegister(0x08);
    year = getRegister(0x09);
    century = getRegister(RTC_CURRENT_CENTURY);

    while ((last_second != second) || (last_minute != minute) ||
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
        while (getUpdateInProgress());

        second = getRegister(0x00);
        minute = getRegister(0x02);
        hour = getRegister(0x04);
        day = getRegister(0x07);
        month = getRegister(0x08);
        year = getRegister(0x09);
        if (RTC_CURRENT_CENTURY != 0) {
            century = getRegister(RTC_CURRENT_CENTURY);
        }
    }

    registerB = getRegister(0x0B);

    // Convert BCD to binary values if necessary
    if (!(registerB & 0x04)) {
        second = (second & 0x0F) + ((second / 16) * 10);
        minute = (minute & 0x0F) + ((minute / 16) * 10);
        hour = ((hour & 0x0F) + (((hour & 0x70) / 16) * 10)) | (hour & 0x80);
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
    if (RTC_CURRENT_CENTURY != 0) {
        year += century * 100;
    } else {
        year += (RTC_CURRENT_YEAR / 100) * 100;
        if (year < RTC_CURRENT_YEAR)
            year += 100;
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
    read();
    return getUnixEpoch(second, minute, hour, day, month, year);
}

}
