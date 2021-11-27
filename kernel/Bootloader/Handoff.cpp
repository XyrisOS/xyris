/**
 * @file Handoff.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Bootloader information parsing and kernel handoff container
 * @version 0.1
 * @date 2021-06-02
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#include <Arch/Arch.hpp>
#include <Bootloader/Arguments.hpp>
#include <Bootloader/Handoff.hpp>
// System library functions
#include <Library/stdio.hpp>
#include <Library/string.hpp>
#include <Memory/paging.hpp>
#include <Panic.hpp>
// Generic devices
#include <Devices/graphics/console.hpp>
#include <Devices/serial/rs232.hpp>
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
    : m_handle(NULL)
    , m_magic(0)
{
    // Initialize nothing.
}

Handoff::Handoff(void* handoff, uint32_t magic)
    : m_handle(handoff)
    , m_magic(magic)
{
    // Parse the handle based on the magic
    debugf("Bootloader info at 0x%p\n", handoff);
    if (magic == 0x36d76289) {
        m_bootType = Multiboot2;
        parseMultiboot2(this, handoff);
    } else if (magic == *(uint32_t*)"stv2") {
        m_bootType = Stivale2;
        parseStivale2(this, handoff);
    } else {
        panic("Invalid bootloader magic!");
    }
}

/*
 *  ___ _   _          _     ___
 * / __| |_(_)_ ____ _| |___|_  )
 * \__ \  _| \ V / _` | / -_)/ /
 * |___/\__|_|\_/\__,_|_\___/___|
 */

void Handoff::parseStivale2(Handoff* that, void* handoff)
{
    debugf("Booted via Stivale2\n");
    // Walk the list of tags in the header
    struct stivale2_struct* fixed = (struct stivale2_struct*)handoff;
    struct stivale2_tag* tag = (struct stivale2_tag*)(fixed->tags);
    while (tag) {
        switch (tag->identifier) {
            case STIVALE2_STRUCT_TAG_MEMMAP_ID: {
                auto memmap = (struct stivale2_struct_tag_memmap*)tag;
                debugf("Found %Lu Stivale2 memmap entries.\n", memmap->entries);
                if (memmap->entries > that->m_memoryMap.Count())
                    panic("Not enough space to add all memory map entries!");
                // Follows the tag list order in stivale2.h
                for (size_t i = 0; i < memmap->entries; i++) {
                    auto entry = memmap->memmap[i];
                    uint64_t end = entry.base + entry.length - 1;
                    that->m_memoryMap[i] = Memory::Section(entry.base, end);
                    debugf("[%zu] 0x%08LX-0x%08LX 0x%08LX\n", i, entry.base, end, entry.length);
                    // TODO: Make this a map that can be indexed
                    switch (entry.type) {
                        case STIVALE2_MMAP_USABLE:
                            that->m_memoryMap[i].setType(Memory::Available);
                            break;

                        case STIVALE2_MMAP_RESERVED:
                            that->m_memoryMap[i].setType(Memory::Reserved);
                            break;

                        case STIVALE2_MMAP_ACPI_RECLAIMABLE:
                            that->m_memoryMap[i].setType(Memory::ACPI);
                            break;

                        case STIVALE2_MMAP_ACPI_NVS:
                            that->m_memoryMap[i].setType(Memory::NVS);
                            break;

                        case STIVALE2_MMAP_BAD_MEMORY:
                            that->m_memoryMap[i].setType(Memory::Bad);
                            break;

                        case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE:
                            that->m_memoryMap[i].setType(Memory::Bootloader);
                            break;

                        case STIVALE2_MMAP_KERNEL_AND_MODULES:
                            that->m_memoryMap[i].setType(Memory::Kernel);
                            break;

                        default:
                            that->m_memoryMap[i].setType(Memory::Unknown);
                            break;
                    }
                }
                break;
            }
            case STIVALE2_STRUCT_TAG_CMDLINE_ID: {
                auto cmdline = (struct stivale2_struct_tag_cmdline*)tag;
                that->m_cmdline = (char*)(cmdline->cmdline);
                debugf("Stivale2 cmdline: '%s'\n", that->m_cmdline);
                parseCommandLine(that->m_cmdline);
                break;
            }
            case STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID: {
                auto framebuffer = (struct stivale2_struct_tag_framebuffer*)tag;
                debugf("Stivale2 framebuffer:\n");
                debugf("\tAddress: 0x%08LX\n", framebuffer->framebuffer_addr);
                debugf("\tResolution: %ux%ux%u\n",
                    framebuffer->framebuffer_width,
                    framebuffer->framebuffer_height,
                    framebuffer->framebuffer_bpp);
                debugf("\tPixel format:\n"
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
                that->m_framebuffer = Graphics::Framebuffer(
                    framebuffer->framebuffer_width,
                    framebuffer->framebuffer_height,
                    framebuffer->framebuffer_bpp,
                    framebuffer->framebuffer_pitch,
                    reinterpret_cast<void*>(framebuffer->framebuffer_addr),
                    static_cast<Graphics::FramebufferMemoryModel>(framebuffer->memory_model),
                    framebuffer->red_mask_size,
                    framebuffer->red_mask_shift,
                    framebuffer->green_mask_size,
                    framebuffer->green_mask_shift,
                    framebuffer->blue_mask_size,
                    framebuffer->blue_mask_shift);
                break;
            }
            default: {
                // debugf("Unknown Stivale2 tag: 0x%016X\n", tag->identifier);
                break;
            }
        }

        tag = (struct stivale2_tag*)tag->next;
    }
    debugf("Done\n");
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
    debugf("Booted via Multiboot2\n");
    auto fixed = (struct multiboot_fixed*)handoff;
    struct multiboot_tag* tag = (struct multiboot_tag*)((uintptr_t)fixed + sizeof(struct multiboot_fixed));
    while (tag->type != MULTIBOOT_TAG_TYPE_END) {
        switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_CMDLINE: {
                auto cmdline = (struct multiboot_tag_string*)tag;
                that->m_cmdline = (char*)(cmdline->string);
                debugf("Multiboot2 cmdline: '%s'\n", that->m_cmdline);
                parseCommandLine(that->m_cmdline);
                break;
            }
            case MULTIBOOT_TAG_TYPE_MMAP: {
                size_t memMapIdx = 0;
                auto memmap = (struct multiboot_tag_mmap*)tag;
                debugf("Multiboot2 memmap found...\n");
                // Because the Multiboot2 header typedefs literally everything as something
                // other than what's in stdint.h and stddef.h, we have to do a ton of casts
                for (multiboot_memory_map_t* entry = memmap->entries;
                     (uint8_t*)entry < (uint8_t*)tag + tag->size;
                     entry = (multiboot_memory_map_t*)((uintptr_t)entry + memmap->entry_size)) {
                    if (memMapIdx > that->m_memoryMap.Count()) {
                        panic("Not enough space to add all memory map entries!");
                    }

                    uintptr_t end = entry->addr + entry->len - 1;
                    that->m_memoryMap[memMapIdx] = Memory::Section(entry->addr, end);
                    // TODO: Make this a map that can be indexed
                    switch (entry->type) {
                        case MULTIBOOT_MEMORY_AVAILABLE:
                            that->m_memoryMap[memMapIdx].setType(Memory::Available);
                            break;
                        case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE:
                            that->m_memoryMap[memMapIdx].setType(Memory::ACPI);
                            break;
                        case MULTIBOOT_MEMORY_NVS:
                            that->m_memoryMap[memMapIdx].setType(Memory::NVS);
                            break;
                        case MULTIBOOT_MEMORY_BADRAM:
                            that->m_memoryMap[memMapIdx].setType(Memory::Bad);
                            break;
                        default:
                            that->m_memoryMap[memMapIdx].setType(Memory::Reserved);
                            break;
                    }

                    memMapIdx++;
                }
                break;
            }
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER: {
                auto framebuffer = (struct multiboot_tag_framebuffer*)tag;
                debugf("Multiboot2 framebuffer:\n");
                debugf("\tAddress: 0x%08LX\n", framebuffer->common.framebuffer_addr);
                debugf("\tResolution: %ux%ux%u\n",
                    framebuffer->common.framebuffer_width,
                    framebuffer->common.framebuffer_height,
                    (framebuffer->common.framebuffer_bpp));
                debugf("\tPixel format:\n"
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
                that->m_framebuffer = Graphics::Framebuffer(
                    framebuffer->common.framebuffer_width,
                    framebuffer->common.framebuffer_height,
                    framebuffer->common.framebuffer_bpp,
                    framebuffer->common.framebuffer_pitch,
                    (void*)framebuffer->common.framebuffer_addr,
                    (Graphics::FramebufferMemoryModel)framebuffer->common.framebuffer_type,
                    framebuffer->framebuffer_red_mask_size,
                    framebuffer->framebuffer_red_field_position,
                    framebuffer->framebuffer_green_mask_size,
                    framebuffer->framebuffer_green_field_position,
                    framebuffer->framebuffer_blue_mask_size,
                    framebuffer->framebuffer_blue_field_position);
                break;
            }
            default: {
                // debugf("Unknown Multiboot2 tag: 0x%08X\n", tag->type);
                break;
            }
        }
        // move to the next tag, aligning if necessary
        tag = (struct multiboot_tag*)(((uintptr_t)tag + tag->size + 7) & ~((uintptr_t)7));
    }
}

} // !namespace Handoff
