/**
 * @file rtc.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2019-11-18
 *
 * @copyright Copyright the Panix Contributors (c) 2019
 *
 */
#pragma once

#include <stdint.h>

#define RTC_CMOS_PORT       0x70
#define RTC_DATA_PORT       0x71
#define RTC_CURRENT_YEAR    2019    // Needs to be updated every year!
#define RTC_CURRENT_CENTURY 0       // Needs to be updated every century!

extern uint32_t rtc_second; // Current UTC second
extern uint32_t rtc_minute; // Current UTC minute
extern uint32_t rtc_hour;   // Current UTC hour
extern uint32_t rtc_day;    // Current UTC day (not reliable)
extern uint32_t rtc_month;  // Current UTC month
extern uint32_t rtc_year;  // Current UTC year

/**
 * @brief Initializes the Real Time Clock driver
 * for the x86_64 architecture.
 *
 */
void rtc_init();
/**
 * @brief Prints the current time as specified by
 * the platforms Real Time Clock.
 *
 */
void rtc_print();
