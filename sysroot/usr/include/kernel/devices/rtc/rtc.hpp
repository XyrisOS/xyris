/**
 * @file rtc.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.1
 * @date 2019-11-18
 * 
 * @copyright Copyright Keeton Feavel et al (c) 2019
 * 
 */

#ifndef PANIX_RTC_HPP
#define PANIX_RTC_HPP

#include <sys/sys.hpp>

#define RTC_CMOS_PORT       0x70
#define RTC_DATA_PORT       0x71
#define RTC_CURRENT_YEAR    2019    // Needs to be updated every year!
#define RTC_CURRENT_CENTURY 0       // Needs to be updated every century!

extern uint8_t second; // Current UTC second
extern uint8_t minute; // Current UTC minute
extern uint8_t hour;   // Current UTC hour
extern uint8_t day;    // Current UTC day (not reliable)
extern uint8_t month;  // Current UTC month
extern uint32_t year;  // Current UTC year

void px_rtc_init();
void px_rtc_print();

#endif /* PANIX_RTC_CPP */