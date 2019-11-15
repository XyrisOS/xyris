/**
 * @file smbios.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.1
 * @date 2019-11-15
 * 
 * @copyright Copyright Keeton Feavel et al (c) 2019
 * 
 */

#include <devices/smbios/smbios.hpp>

char* px_get_smbios_addr() {
    uintptr_t *mem = (uintptr_t *) 0xF0000;
    int length, i;
    unsigned char checksum;
    while ((uintptr_t) mem < 0x100000) {
        if (mem[0] == '_' && mem[1] == 'S' && mem[2] == 'M' && mem[3] == '_') {
            length = mem[5];
            checksum = 0;
            for(i = 0; i < length; i++) {
                checksum += mem[i];
            }
            if(checksum == 0) break;
        }
        mem += 16;
    }
    // Perform a check and see if we found the SMBIOS.
    ((unsigned int) mem == 0x100000) ?  px_print_debug("No SMBIOS found!", Error) : px_print_debug("SMBIOS located.", Success);
}