/**
 * @file timer.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Programmable Interrupt Timer driver functions
 * @version 0.3
 * @date 2019-11-15
 *
 * @copyright Copyright the Xyris Contributors (c) 2019
 *
 */
#include <arch/i386/timer.hpp>
#include <lib/stdio.hpp>
#include <lib/string.hpp>
#include <dev/graphics/tty.hpp>

static void timer_callback(struct registers *regs);
volatile uint32_t timer_tick;

typedef void (*voidfunc_t)();

#define MAX_CALLBACKS 8
static size_t _callback_count = 0;
static voidfunc_t _callbacks[MAX_CALLBACKS];

/**
 * Sleep Timer Non-Busy Waiting Idea:
 * Create a struct that contains the end time and the callback
 * function pointer that should be called when timer_tick = end
 * After each timer_tick we check our end time and call the function
 * if we're equal.
 */

void timer_init(uint32_t freq) {
    /* Install the function we just wrote */
    register_interrupt_handler(IRQ0, timer_callback);
    /* Get the PIT value: hardware clock at 1193180 Hz */
    uint32_t divisor = 1193180 / freq;
    uint8_t low  = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);
    /* Send the command */
    writeByte(TIMER_COMMAND_PORT, 0x36);
    writeByte(TIMER_DATA_PORT, low);
    writeByte(TIMER_DATA_PORT, high);
}

static void timer_callback(struct registers *regs) {
    (void)regs;
    timer_tick++;
    for (size_t i = 0; i < _callback_count; i++) {
        _callbacks[i]();
    }
}

void timer_print() {
    kprintf(DBG_INFO "Tick: %i\n", timer_tick);
}

void sleep(uint32_t ms) {
    uint32_t start = timer_tick;
    uint32_t final = start + ms;
    // Waste CPU cycles like a slob
    while (timer_tick < final);
    // Return now that we've waited long enough
    return;
}

void timer_register_callback(void (*func)()) {
    if (_callback_count < MAX_CALLBACKS - 1) {
        _callbacks[_callback_count] = func;
        _callback_count++;
    }
}
