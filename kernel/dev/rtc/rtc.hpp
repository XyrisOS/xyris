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
#pragma once
#include <stdint.h>

namespace RTC {

/**
 * @brief Initializes the Real Time Clock driver
 * for the x86_64 architecture.
 *
 */
void init();

/**
 * @brief Get a epoch number from rtc.
 * @return uint64_t A epoch number
 */
uint64_t getEpoch();

} // !namespace RTC
