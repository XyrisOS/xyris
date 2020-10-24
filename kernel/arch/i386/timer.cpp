/**
 * @file timer.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Programmable Interrupt Timer driver functions
 * @version 0.3
 * @date 2019-11-15
 *
 * @copyright Copyright the Panix Contributors (c) 2019
 *
 */

#include <arch/arch.hpp>
#include <lib/stdio.hpp>
#include <lib/string.hpp>
#include <dev/tty/tty.hpp>

static void px_timer_callback(registers_t *regs);
volatile uint32_t px_timer_tick;

typedef void (*voidfunc_t)();

#define MAX_CALLBACKS 8
static size_t _callback_count = 0;
static voidfunc_t _callbacks[MAX_CALLBACKS];

/**
 * Sleep Timer Non-Busy Waiting Idea:
 * Create a struct that contains the end time and the callback
 * function pointer that should be called when px_timer_tick = end
 * After each px_timer_tick we check our end time and call the function
 * if we're equal.
 */

void px_timer_init(uint32_t freq) {
    px_kprintf(DBG_INFO "Initializing timer\n");
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
    px_kprintf(DBG_OKAY "Started timer\n");
}

static void px_timer_callback(registers_t *regs) {
    px_timer_tick++;
    for (size_t i = 0; i < _callback_count; i++) {
        _callbacks[i]();
    }
}

void px_timer_print() {
    px_kprintf(DBG_INFO "Tick: %i\n", px_timer_tick);
}

void sleep(uint32_t ms) {
    uint32_t start = px_timer_tick;
    uint32_t final = start + ms;
    // Waste CPU cycles like a slob
    while (px_timer_tick < final);
    // Return now that we've waited long enough
    return;
}

void px_timer_register_callback(void (*func)()) {
    if (_callback_count < MAX_CALLBACKS - 1) {
        _callbacks[_callback_count] = func;
        _callback_count++;
    }
}
