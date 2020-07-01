/**
 * @file timer.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.1
 * @date 2019-11-15
 *
 * @copyright Copyright Keeton Feavel et al (c) 2019
 *
 */

#include <arch/arch.hpp>
#include <lib/string.hpp>

static void px_timer_callback(registers_t *regs);
uint32_t tick;

/**
 * Sleep Timer Non-Busy Waiting Idea:
 * Create a struct that contains the end time and the callback
 * function pointer that should be called when tick = end
 * After each tick we check our end time and call the function
 * if we're equal.
 */

void px_timer_init(uint32_t freq) {
    px_print_debug("Initializing timer", Info);
    /* Install the function we just wrote */
    px_register_interrupt_handler(IRQ0, px_timer_callback);
    /* Get the PIT value: hardware clock at 1193180 Hz */
    uint32_t divisor = 1193180 / freq;
    uint8_t low  = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);
    /* Send the command */
    px_write_byte(TIMER_COMMAND_PORT, 0x36);
    px_write_byte(TIMER_DATA_PORT, low);
    px_write_byte(TIMER_DATA_PORT, high);
    px_print_debug("Started timer", Success);
}

static void px_timer_callback(registers_t *regs) {
    tick++;
}

void px_timer_print() {
    char tick_ascii[256];
    itoa(tick, tick_ascii);
    px_print_debug(tick_ascii, Info);
}

void sleep(uint32_t ms) {
    uint32_t start = tick;
    uint32_t final = start + ms;
    // Waste CPU cycles like a slob
    while (tick != final);
    // Return now that we've waited long enough
    return;
}