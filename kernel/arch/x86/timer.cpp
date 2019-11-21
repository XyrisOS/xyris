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

#include <arch/i386/timer.hpp>
#include <arch/i386/isr.hpp>    // Needed to register our callback

static void px_timer_callback(registers_t regs);
uint32_t tick;

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

static void px_timer_callback(registers_t regs) {
    tick++;
}

void px_timer_print() {
    char tick_ascii[256];
    itoa(tick, tick_ascii);
    px_print_debug(tick_ascii, Info);
}