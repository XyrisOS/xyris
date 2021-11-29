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
#include <Arch/Arch.hpp>
/*
// TODO: Figure out why these structs aren't the same as the ones in stivale2.s
#define STIVALE2_MAGIC "stv2"

USED SECTION(".early_text")
static void stivale2_entry(struct stivale2_struct *info)
{
    kernelEntry((void*)info, (uint32_t)STIVALE2_MAGIC);
}

USED ALIGN(4) SECTION(".early_data")
static struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = 0,
    },
    .framebuffer_width  = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp    = 0,
};

USED ALIGN(4) SECTION(".stivale2hdr")
static struct stivale2_header stivale_hdr = {
    .entry_point = 0,
    .stack = EARLY_BSS_END,
    .flags = 0,
    .tags = (uintptr_t)&framebuffer_hdr_tag,
};
*/
