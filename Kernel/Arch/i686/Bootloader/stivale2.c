/**
 * @file stivale.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Stivale2 bootloader header and stage 1 handler.
 * @version 0.1
 * @date 2021-09-11
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */

#include <stdint.h>
#include <Support/sections.hpp>
#include <stivale/stivale2.h>
#include <Arch/i686/Bootloader/EarlyPanic.hpp>
#include <Arch/i686/Bootloader/Loader.hpp>

#define STIVALE2_MAGIC "stv2"

// TODO: Remove once boot.s is gone
uint32_t stivale2_mmap_helper(void* baseaddr);

/**
 * @brief Scan the stivale2 tags to find the reclaimable bootloader
 * memory map tag that starts at the stivale2 tag base address. Return
 * the length found. We'll memory map the entire area.
 *
 */
uint32_t
__attribute__((section(".early_text")))
stivale2_mmap_helper(void* baseaddr)
{
    struct stivale2_tag* tag = (struct stivale2_tag*)baseaddr;

    while (tag) {
        switch (tag->identifier) {
            case STIVALE2_STRUCT_TAG_MEMMAP_ID: {
                struct stivale2_struct_tag_memmap* memmap = (struct stivale2_struct_tag_memmap*)tag;
                for (size_t i = 0; i < memmap->entries; i++) {
                    switch (memmap->memmap[i].type) {
                        case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE:
                            if (((uintptr_t)memmap->memmap[i].base) == (uintptr_t)baseaddr) {
                                return (uintptr_t)memmap->memmap[i].length;
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
        tag = (struct stivale2_tag*)(uint32_t)tag->next;
    }
    /* If we get here, there's a problem so we will just
       return 0 and panic in boot.S */
    EarlyPanic("Failed to find bootloader memory region!\n");
}

/**
 * @brief Stivale2 protocol kernel stage 1 entry. Stage 1 is responsible
 * for providing an entry point for the bootloader in C, performing any
 * necessary bootstrappign and then calling into the C++ stage 2.
 *
 */
__attribute__((section(".early_text")))
static void stage1Entry(struct stivale2_struct *info)
{
    stage2Entry(info, (uint32_t)STIVALE2_MAGIC);
}

/*
 *   ___ _   _          _     ___   ___ _               _
 *  / __| |_(_)_ ____ _| |___|_  ) / __| |_ _ _ _  _ __| |_ _  _ _ _ ___ ___
 *  \__ \  _| \ V / _` | / -_)/ /  \__ \  _| '_| || / _|  _| || | '_/ -_|_-<
 *  |___/\__|_|\_/\__,_|_\___/___| |___/\__|_|  \_,_\__|\__|\_,_|_| \___/__/
 */

struct stivale2_header_i686 {
    uint32_t entry_point;
    uint32_t unused_1;
    uint32_t stack;
    uint32_t unused_2;
    uint32_t flags;
    uint32_t unused_3;
    uint32_t tags;
    uint32_t unused_4;
} __attribute__((__packed__));

__attribute__((aligned(4)))
__attribute__((section(".early_data")))
const struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = 0,
    },
    .framebuffer_width  = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp    = 0,
};

__attribute__((aligned(4)))
__attribute__((section(".stivale2hdr")))
const struct stivale2_header_i686 stivale_hdr = {
    .entry_point = (uint32_t)(void*)stage1Entry,
    .stack = (uint32_t)EARLY_BSS_END,
    .flags = 0,
    .tags = (uint32_t)&framebuffer_hdr_tag,
};
