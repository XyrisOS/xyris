/**
 * @file smbios.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief The SMBIOS is a table located in the BIOS that contains
 * information about the hardware. These functions are incomplete
 * because once we find the information we don't really do anything
 * with it. It still needs to be loaded into the struct to be
 * accessed later.
 * @version 0.1
 * @date 2019-11-15
 * 
 * @copyright Copyright Keeton Feavel et al (c) 2019
 * 
 */

#include <devices/smbios/smbios.hpp>

char* px_get_smbios_addr() {
    // Start at a location above the 0x10000 location and
    // continue searching for the SMBIOS information
    uintptr_t *mem = (uintptr_t *) 0xF0000;
    int length, i;
    unsigned char checksum;
    // 0x100000 is the start of non-BIOS reserved memory
    // So we can't read anything after that point.
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
    return (char *)mem;
}

size_t px_get_smbios_table_len(struct SMBIOSHeader *hd) {
    size_t i;
    const char *strtab = (char *)hd + hd->len;
    // Scan until we find a double zero byte
    for (i = 1; strtab[i - 1] != '\0' || strtab[i] != '\0'; i++) {
        //
    }
    return hd->len + i + 1;
}