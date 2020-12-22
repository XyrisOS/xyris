/**
 * @file spkr.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2019-11-25
 *
 * @copyright Copyright the Panix Contributors (c) 2019
 *
 */
#pragma once

#include <stdint.h>

/**
 * @brief Plays a given frequency for the provided duration.
 * 
 * @param freq Tone frequency
 * @param ms Length in milliseconds
 */
void px_spkr_beep(uint32_t freq, uint32_t ms);
