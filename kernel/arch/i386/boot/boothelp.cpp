/**
 * @file boothelp.cpp
 * @author James T. Sprinkle (the-grue@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2021-08-07
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#include <stdint.h>
#include <meta/sections.hpp>
#include <meta/compiler.hpp>
#include <multiboot/multiboot2.h>
#include <stivale/stivale2.h>

extern "C" uint32_t stivale2_mmap_helper(void* baseaddr);
extern "C" uint32_t multiboot2_mmap_helper(void* baseaddr);

/**
 *  ___ _   _          _     ___
 * / __| |_(_)_ ____ _| |___|_  )
 * \__ \  _| \ V / _` | / -_)/ /
 * |___/\__|_|\_/\__,_|_\___/___|
 *
 * Scan the stivale2 tags to find the reclaimable bootloader
 * memory map tag that starts at the stivale2 tag base
 * address.  Return the length found.  We'll memory map
 * the entire area.
 *
 */
extern "C" uint32_t
OPTIMIZE(0)
SECTION(".early_text")
stivale2_mmap_helper(void* baseaddr)
{
    struct stivale2_tag* tag = (struct stivale2_tag*)baseaddr;

    while (tag) {
        switch (tag->identifier) {
        case STIVALE2_STRUCT_TAG_MEMMAP_ID: {
            auto memmap = (struct stivale2_struct_tag_memmap*)tag;
            for (uint32_t i = 0; i < (uint32_t)memmap->entries; i++) {
                switch ((uint32_t)memmap->memmap[i].type) {
                case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE:
                    if (((uint32_t)memmap->memmap[i].base) == (uint32_t)baseaddr) {
                        return (uint32_t)memmap->memmap[i].length;
                    }
                    break;

                default:
                    break;
                }
            }
        }
        default:
            break;
        }
        tag = (struct stivale2_tag*)tag->next;
    }
    // If we get here, there's a problem so we will just
    // return 0 and panic in boot.S
    return 0;
}

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

extern "C" uint32_t
OPTIMIZE(0)
SECTION(".early_text")
multiboot2_mmap_helper(void* baseaddr)
{
    struct multiboot_fixed {
        uint32_t total_size;
        uint32_t reserved;
    };

    auto fixed = (struct multiboot_fixed*)baseaddr;
    return fixed->total_size;
}
