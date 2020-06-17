/**
 * @file spkr.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.1
 * @date 2019-11-25
 * 
 * @copyright Copyright Keeton Feavel et al (c) 2019
 * 
 */

#ifndef PANIX_DEVICE_SPKR_HPP
#define PANIX_DEVICE_SPKR_HPP

#include <sys/sys.hpp>

void px_spkr_beep(uint32_t freq, uint32_t ms);
static void px_spkr_tone(uint32_t freq);
static void px_spkr_stop();

#endif /* PANIX_DEVICE_SPKR_HPP */