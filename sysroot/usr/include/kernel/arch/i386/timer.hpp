/**
 * @file timer.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.1
 * @date 2019-11-15
 * 
 * @copyright Copyright Keeton Feavel et al (c) 2019
 * 
 */

#ifndef PANIX_I386_TIMER_HPP
#define PANIX_I386_TIMER_HPP

#include <sys/sys.hpp>
#define TIMER_COMMAND_PORT 0x43
#define TIMER_DATA_PORT 0x40

/**
 * @brief Initialize the CPU timer with the given frequency.
 * 
 * @param freq Timer frequency
 */
void px_timer_init(uint32_t freq);
/**
 * @brief Prints out the current tick.
 * 
 */
void px_timer_print();

#endif /* PANIX_I386_TIMER_HPP */