/**
 * @file spkr.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2019-11-25
 *
 * @copyright Copyright the Xyris Contributors (c) 2019
 *
 */

#include <arch/Arch.hpp>
#include <dev/spkr/spkr.hpp>
#include <arch/i386/timer.hpp> // TODO: Remove ASAP

static void spkr_tone(uint32_t freq) {
    uint32_t div;
    uint8_t tmp;
    // Set the PIT to the desired frequency
    div = 1193180 / freq;
    writeByte(0x43, 0xb6);
    writeByte(0x42, (uint8_t)(div));
    writeByte(0x42, (uint8_t)(div >> 8));
    // And play the sound using the PC speaker
    tmp = readByte(0x61);
    if (tmp != (tmp | 3)) {
        writeByte(0x61, tmp | 3);
    }
}

static void spkr_stop() {
    uint8_t tmp = readByte(0x61) & 0xFC;
    writeByte(0x61, tmp);
}

void spkr_beep(uint32_t freq, uint32_t ms) {
    spkr_tone(freq);
    // TODO: Change this to task sleep
    sleep(ms);
    spkr_stop();
    // set_PIT_2(old_frequency);
}