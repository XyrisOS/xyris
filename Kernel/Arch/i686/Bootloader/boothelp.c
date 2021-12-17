/**
 * @file boothelp.cpp
 * @author James T. Sprinkle (the-grue@hotmail.com)
 * @brief Maps bootloader information into memory
 * @version 0.1
 * @date 2021-08-07
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */

#include <Support/sections.hpp>
#include <multiboot/multiboot2.h>
#include <stdint.h>
#include <stivale/stivale2.h>

uint32_t multiboot2_mmap_helper(void* baseaddr);

/*
 *  __  __      _ _   _ _              _   ___
 * |  \/  |_  _| | |_(_) |__  ___  ___| |_|_  )
 * | |\/| | || | |  _| | '_ \/ _ \/ _ \  _|/ /
 * |_|  |_|\_,_|_|\__|_|_.__/\___/\___/\__/___|
 *
 * Multiboot2 has the size of the bootloader info area
 * right at the beginning, which makes things easier.
 * Return the length found.  We'll memory map
 * the entire area.
 *
 */
uint32_t
__attribute__((section(".early_text")))
multiboot2_mmap_helper(void* baseaddr)
{
    struct multiboot_fixed {
        uint32_t total_size;
        uint32_t reserved;
    };

    struct multiboot_fixed* fixed = (struct multiboot_fixed*)baseaddr;
    return fixed->total_size;
}
