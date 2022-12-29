/**
 * @file timer.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2019-11-15
 *
 * @copyright Copyright the Xyris Contributors (c) 2019
 *
 */
#pragma once
#include <Arch/i686/Arch.hpp>
#include <stdint.h>

#define TIMER_COMMAND_PORT 0x43
#define TIMER_DATA_PORT 0x40

extern volatile uint32_t timer_tick;

/**
 * @brief Initialize the CPU timer with the given frequency.
 *
 * @param freq Timer frequency
 */
void timer_init(uint32_t freq);
/**
 * @brief Sleeps for a certain length of time.
 *
 * @param ms Sleep length in milliseconds
 */
void sleep(uint32_t ms);

void timer_register_callback(void (*func)());
