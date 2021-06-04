/**
 * @file Handoff.cpp
 * @author Keeton Feave (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.1
 * @date 2021-06-02
 * 
 * @copyright Copyright the Panix Contributors (c) 2021
 * 
 */
#include <boot/Handoff.hpp>
#include <lib/stdio.hpp>
#include <lib/string.hpp>
#include <mem/paging.hpp>
#include <dev/serial/rs232.hpp>

#include <multiboot/multiboot2.h>
#include <stivale/stivale2.h>

namespace Boot {

/*
 *  ___                   _          __  __
 * | __| _ __ _ _ __  ___| |__ _  _ / _|/ _|___ _ _
 * | _| '_/ _` | '  \/ -_) '_ \ || |  _|  _/ -_) '_|
 * |_||_| \__,_|_|_|_\___|_.__/\_,_|_| |_| \___|_|
 *
 */

FramebufferInfo::FramebufferInfo()
    : _addr(NULL)
    , _width(0)
    , _height(0)
    , _depth(0)
    , _pitch(0)
    , _redMaskSize(0)
    , _redMaskShift(0)
    , _greenMaskSize(0)
    , _greenMaskShift(0)
    , _blueMaskSize(0)
    , _blueMaskShift(0)
    , _memoryModel(Undefined_FBMM)
{
    // Default constructor.
}

FramebufferInfo::FramebufferInfo(uint16_t width, uint16_t height, uint16_t depth, uint16_t pitch, void* addr)
    : _addr(addr)
    , _width(width)
    , _height(height)
    , _depth(depth)
    , _pitch(pitch)
    , _redMaskSize(0)
    , _redMaskShift(0)
    , _greenMaskSize(0)
    , _greenMaskShift(0)
    , _blueMaskSize(0)
    , _blueMaskShift(0)
    , _memoryModel(Undefined_FBMM)
{
    // Common parameters constructor
}

FramebufferInfo::FramebufferInfo(uint16_t width, uint16_t height,
                                 uint16_t depth, uint16_t pitch,
                                 void* addr, FramebufferMemoryModel model,
                                 uint8_t redMaskSize, uint8_t redMaskShift,
                                 uint8_t greenMaskSize, uint8_t greenMaskShift,
                                 uint8_t blueMaskSize, uint8_t blueMaskShift)
    : _addr(addr)
    , _width(width)
    , _height(height)
    , _depth(depth)
    , _pitch(pitch)
    , _redMaskSize(redMaskSize)
    , _redMaskShift(redMaskShift)
    , _greenMaskSize(greenMaskSize)
    , _greenMaskShift(greenMaskShift)
    , _blueMaskSize(blueMaskSize)
    , _blueMaskShift(blueMaskShift)
    , _memoryModel(model)
{
    // All parameters constructor
}

/*
 *  _  _              _      __  __
 * | || |__ _ _ _  __| |___ / _|/ _|
 * | __ / _` | ' \/ _` / _ \  _|  _|
 * |_||_\__,_|_||_\__,_\___/_| |_|
 *
 */

Handoff::Handoff(void* handoff, uint32_t magic)
    : _handle(handoff)
    , _magic(magic)
{
    // Parse the handle based on the magic
    if (magic == 0x36d76289) {
        _bootType = Multiboot2;
        parseMultiboot2(handoff);
    } else if (magic == *(uint32_t*)"stv2") {
        _bootType = Stivale2;
        parseStivale2(handoff);
    }
}

Handoff::~Handoff()
{
    // Nothing to deconstruct
}

void Handoff::parseStivale2(void* handoff)
{
    auto fixed = (struct stivale2_struct*)handoff;
    // Walk the list of tags in the header
    auto tag = (struct stivale2_tag*)(fixed->tags);
    while (tag)
    {
        // Map in each tag since Stivale2 doesn't give us a total size like Multiboot does.
        // TODO: Find a way around this when the new memory manager code is done.
        uintptr_t page = ((uintptr_t)tag & PAGE_ALIGN);
        map_kernel_page(VADDR(page), page);
        // Follows the tag list order in stivale2.h
        switch(tag->identifier)
        {
            case STIVALE2_STRUCT_TAG_CMDLINE_ID:
            {
                auto cmdline = (struct stivale2_struct_tag_cmdline*)tag;
                rs232_printf("Stivale2 cmdline: '%s'\n", (const char *)cmdline->cmdline);
                _cmdline = (const char *)(cmdline->cmdline);
                break;
            }
            case STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID:
            {
                auto framebuffer = (struct stivale2_struct_tag_framebuffer*)tag;
                rs232_printf("Stivale2 framebuffer:\n");
                rs232_printf("\tAddress: 0x%08X", framebuffer->framebuffer_addr);
                rs232_printf("\tResolution: %ix%ix%i\n",
                    framebuffer->framebuffer_width,
                    framebuffer->framebuffer_height,
                    (framebuffer->framebuffer_bpp * 8));
                // Initialize the framebuffer information
                _fbInfo = FramebufferInfo(
                    framebuffer->framebuffer_width,
                    framebuffer->framebuffer_height,
                    framebuffer->framebuffer_bpp,
                    framebuffer->framebuffer_pitch,
                    (void*)framebuffer->framebuffer_addr
                );
                break;
            }
            case STIVALE2_STRUCT_TAG_EPOCH_ID:
            {
                auto epoch = (struct stivale2_struct_tag_epoch*)tag;
                rs232_printf("Stivale2 epoch: %i\n", epoch->epoch);
                break;
            }
            case STIVALE2_STRUCT_TAG_FIRMWARE_ID:
            {
                auto firmware = (struct stivale2_struct_tag_firmware*)tag;
                rs232_printf("Stivale2 firmware flags: 0x%08X\n", firmware->flags);
                rs232_printf("\tBooted using %s\n", (firmware->flags & 0x1 ? "BIOS" : "UEFI"));
                break;
            }
            default:
            {
                rs232_printf("Unknown Stivale2 tag: %d\n", tag->identifier);
                break;
            }
        }

        tag = (struct stivale2_tag*)tag->next;
    }
    rs232_printf("Done\n");
}

void Handoff::parseMultiboot2(void* handoff)
{
    (void)handoff;
}

};
