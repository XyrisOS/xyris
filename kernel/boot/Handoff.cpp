/**
 * @file Handoff.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.1
 * @date 2021-06-02
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#include <boot/Handoff.hpp>
#include <boot/Arguments.hpp>
// System library functions
#include <lib/stdio.hpp>
#include <lib/string.hpp>
#include <mem/paging.hpp>
// Generic devices
#include <dev/graphics/tty.hpp>
#include <dev/serial/rs232.hpp>
// Bootloaders
#include <multiboot/multiboot2.h>
#include <stivale/stivale2.h>

namespace Boot {

/*
 *  _  _              _      __  __
 * | || |__ _ _ _  __| |___ / _|/ _|
 * | __ / _` | ' \/ _` / _ \  _|  _|
 * |_||_\__,_|_||_\__,_\___/_| |_|
 *
 */

Handoff::Handoff()
    : _handle(NULL)
    , _magic(0)
{
    // Initialize nothing.
}

Handoff::Handoff(void* handoff, uint32_t magic)
    : _handle(handoff)
    , _magic(magic)
{
    const char* bootProtoName;
    // Parse the handle based on the magic
    RS232::printf("Bootloader info at 0x%X\n", handoff);
    if (magic == 0x36d76289) {
        bootProtoName = "Multiboot2";
        _bootType = Multiboot2;
        parseMultiboot2(this, handoff);
    } else if (magic == *(uint32_t*)"stv2") {
        bootProtoName = "Stivale2";
        _bootType = Stivale2;
        parseStivale2(this, handoff);
    } else {
        PANIC("Invalid bootloader information!");
    }
    RS232::printf("Booted via %s\n", bootProtoName);
}

Handoff::~Handoff()
{
    // Nothing to deconstruct
}

/*
 *  ___ _   _          _     ___
 * / __| |_(_)_ ____ _| |___|_  )
 * \__ \  _| \ V / _` | / -_)/ /
 * |___/\__|_|\_/\__,_|_\___/___|
 */

void Handoff::parseStivale2(Handoff* that, void* handoff)
{
    struct stivale2_struct* fixed = (struct stivale2_struct*)handoff;
    // Walk the list of tags in the header
    struct stivale2_tag* tag = (struct stivale2_tag*)(fixed->tags);
    while (tag) {
         // Follows the tag list order in stivale2.h
        switch (tag->identifier) {
#ifdef DEBUG
        case STIVALE2_STRUCT_TAG_MEMMAP_ID: {
            auto memmap = (struct stivale2_struct_tag_memmap*)tag;
            RS232::printf("Stivale2 memmap found...\n");
            for (uint32_t i = 0; i < (uint32_t)memmap->entries; i++) {
                switch ((uint32_t)memmap->memmap[i].type) {
                case STIVALE2_MMAP_USABLE:
                    RS232::printf("Stivale2 USABLE memory: ");
                    break;

                case STIVALE2_MMAP_RESERVED:
                    RS232::printf("Stivale2 RESERVED memory: ");
                    break;

                case STIVALE2_MMAP_ACPI_RECLAIMABLE:
                    RS232::printf("Stivale2 ACPI_RECLAIMABLE memory: ");
                    break;

                case STIVALE2_MMAP_ACPI_NVS:
                    RS232::printf("Stivale2 ACPI_NVS memory: ");
                    break;

                case STIVALE2_MMAP_BAD_MEMORY:
                    RS232::printf("Stivale2 BAD memory: ");
                    break;

                case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE:
                    RS232::printf("Stivale2 BOOTLOADER_RECLAIMABLE memory: ");
                    break;

                case STIVALE2_MMAP_KERNEL_AND_MODULES:
                    RS232::printf("Stivale2 KERNEL_AND_MODULES memory: ");
                    break;

                default:
                    RS232::printf("Unknown Memory Type 0x%08X: ", memmap->memmap[i].type);
                    break;
                }
                RS232::printf("Base: 0x%08X, Length: 0x%08X\n", (uint32_t)memmap->memmap[i].base,
                    (uint32_t)memmap->memmap[i].length);
            }
            break;
        }
#endif
        case STIVALE2_STRUCT_TAG_CMDLINE_ID: {
            auto cmdline = (struct stivale2_struct_tag_cmdline*)tag;
            that->_cmdline = (char*)(cmdline->cmdline);
            RS232::printf("Stivale2 cmdline: '%s'\n", that->_cmdline);
            parseCommandLine(that->_cmdline);
            break;
        }
        case STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID: {
            auto framebuffer = (struct stivale2_struct_tag_framebuffer*)tag;
            RS232::printf("Stivale2 framebuffer:\n");
            RS232::printf("\tAddress: 0x%08X\n", framebuffer->framebuffer_addr);
            RS232::printf("\tResolution: %ix%ix%i\n",
                framebuffer->framebuffer_width,
                framebuffer->framebuffer_height,
                framebuffer->framebuffer_bpp);
            RS232::printf("\tPixel format:\n"
                            "\t\tRed size:    %u\n"
                            "\t\tRed shift:   %u\n"
                            "\t\tGreen size:  %u\n"
                            "\t\tGreen shift: %u\n"
                            "\t\tBlue size:   %u\n"
                            "\t\tBlue shift:  %u\n",
                            framebuffer->red_mask_size,
                            framebuffer->red_mask_shift,
                            framebuffer->green_mask_size,
                            framebuffer->green_mask_shift,
                            framebuffer->blue_mask_size,
                            framebuffer->blue_mask_shift);
            // Initialize the framebuffer information
            that->_framebuffer = graphics::Framebuffer(
                framebuffer->framebuffer_width,
                framebuffer->framebuffer_height,
                framebuffer->framebuffer_bpp,
                framebuffer->framebuffer_pitch,
                reinterpret_cast<void*>(framebuffer->framebuffer_addr),
                static_cast<graphics::FramebufferMemoryModel>(framebuffer->memory_model),
                framebuffer->red_mask_size,
                framebuffer->red_mask_shift,
                framebuffer->green_mask_size,
                framebuffer->green_mask_shift,
                framebuffer->blue_mask_size,
                framebuffer->blue_mask_shift
            );
            break;
        }
        default: {
            //RS232::printf("Unknown Stivale2 tag: 0x%016X\n", tag->identifier);
            break;
        }
        }

        tag = (struct stivale2_tag*)tag->next;
    }
    RS232::printf("Done\n");
}

/*
 *  __  __      _ _   _ _              _   ___
 * |  \/  |_  _| | |_(_) |__  ___  ___| |_|_  )
 * | |\/| | || | |  _| | '_ \/ _ \/ _ \  _|/ /
 * |_|  |_|\_,_|_|\__|_|_.__/\___/\___/\__/___|
 */

struct multiboot_fixed {
    uint32_t total_size;
    uint32_t reserved;
};

void Handoff::parseMultiboot2(Handoff* that, void* handoff)
{
    auto fixed = (struct multiboot_fixed*)handoff;
    struct multiboot_tag* tag = (struct multiboot_tag*)((uintptr_t)fixed + sizeof(struct multiboot_fixed));
    while (tag->type != MULTIBOOT_TAG_TYPE_END) {
        switch (tag->type) {
        case MULTIBOOT_TAG_TYPE_CMDLINE: {
            auto cmdline = (struct multiboot_tag_string*)tag;
            that->_cmdline = (char*)(cmdline->string);
            RS232::printf("Multiboot2 cmdline: '%s'\n", that->_cmdline);
            parseCommandLine(that->_cmdline);
            break;
        }
        case MULTIBOOT_TAG_TYPE_FRAMEBUFFER: {
            auto framebuffer = (struct multiboot_tag_framebuffer *)tag;
            RS232::printf("Multiboot2 framebuffer:\n");
            RS232::printf("\tAddress: 0x%08X\n", framebuffer->common.framebuffer_addr);
            RS232::printf("\tResolution: %ix%ix%i\n",
                framebuffer->common.framebuffer_width,
                framebuffer->common.framebuffer_height,
                (framebuffer->common.framebuffer_bpp));
            RS232::printf("\tPixel format:\n"
                            "\t\tRed size:    %u\n"
                            "\t\tRed shift:   %u\n"
                            "\t\tGreen size:  %u\n"
                            "\t\tGreen shift: %u\n"
                            "\t\tBlue size:   %u\n"
                            "\t\tBlue shift:  %u\n",
                            framebuffer->framebuffer_red_mask_size,
                            framebuffer->framebuffer_red_field_position,
                            framebuffer->framebuffer_green_mask_size,
                            framebuffer->framebuffer_green_field_position,
                            framebuffer->framebuffer_blue_mask_size,
                            framebuffer->framebuffer_blue_field_position);
            // Initialize the framebuffer information
            that->_framebuffer = graphics::Framebuffer(
                framebuffer->common.framebuffer_width,
                framebuffer->common.framebuffer_height,
                framebuffer->common.framebuffer_bpp,
                framebuffer->common.framebuffer_pitch,
                (void*)framebuffer->common.framebuffer_addr,
                (graphics::FramebufferMemoryModel)framebuffer->common.framebuffer_type,
                framebuffer->framebuffer_red_mask_size,
                framebuffer->framebuffer_red_field_position,
                framebuffer->framebuffer_green_mask_size,
                framebuffer->framebuffer_green_field_position,
                framebuffer->framebuffer_blue_mask_size,
                framebuffer->framebuffer_blue_field_position
            );
            break;
        }
        default: {
            //RS232::printf("Unknown Multiboot2 tag: 0x%08X\n", tag->type);
            break;
        }
        }
        // move to the next tag, aligning if necessary
        tag = (struct multiboot_tag*)(((uintptr_t)tag + tag->size + 7) & ~((uintptr_t)7));
    }
}

} // !namespace Handoff
