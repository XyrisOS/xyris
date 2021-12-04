/**
 * @file stivale.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Stivale2 bootloader header
 * @version 0.1
 * @date 2021-09-11
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */

#include <stdint.h>
#include <Support/sections.hpp>
#include <stivale/stivale2.h>

#define STIVALE2_MAGIC "stv2"

extern void kernelEntry(void* info, uint32_t magic);

struct stivale2_header_patched {
    uint32_t entry_point;
    uint32_t unused_1;
    uint32_t stack;
    uint32_t unused_2;
    uint32_t flags;
    uint32_t unused_3;
    uint32_t tags;
    uint32_t unused_4;
} __attribute__((__packed__));

__attribute__((section(".early_text")))
static void stivale2_entry(struct stivale2_struct *info)
{
    // TODO: Fix me lol (port boot.s to C)
    kernelEntry((void*)info, (uint32_t)STIVALE2_MAGIC);
}

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
const struct stivale2_header_patched stivale_hdr = {
    .entry_point = (uint32_t)(void*)stivale2_entry,
    .stack = (uint32_t)EARLY_BSS_END,
    .flags = 0,
    .tags = (uint32_t)&framebuffer_hdr_tag,
};
