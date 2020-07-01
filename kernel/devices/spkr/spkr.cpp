/**
 * @file spkr.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.1
 * @date 2019-11-25
 *
 * @copyright Copyright Keeton Feavel et al (c) 2019
 *
 */

#include <arch/arch.hpp>
#include <devices/spkr/spkr.hpp>

static void px_spkr_tone(uint32_t freq) {
    uint32_t div;
    uint8_t tmp;
    // Set the PIT to the desired frequency
    div = 1193180 / freq;
    px_write_byte(0x43, 0xb6);
    px_write_byte(0x42, (uint8_t)(div));
    px_write_byte(0x42, (uint8_t)(div >> 8));   
    // And play the sound using the PC speaker
    tmp = px_read_byte(0x61);
    if (tmp != (tmp | 3)) {
        px_write_byte(0x61, tmp | 3);
    }
}

static void px_spkr_stop() {
    uint8_t tmp = px_read_byte(0x61) & 0xFC;    
    px_write_byte(0x61, tmp);
}

void px_spkr_beep(uint32_t freq, uint32_t ms) {
    px_spkr_tone(freq);
    sleep(ms);
    px_spkr_stop();
    // set_PIT_2(old_frequency);
}