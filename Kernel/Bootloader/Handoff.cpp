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
#include <Logger.hpp>
#include <Panic.hpp>
// Generic devices
#include <Devices/Graphics/console.hpp>
#include <Devices/Serial/rs232.hpp>
// Bootloaders
#include <stivale/stivale2.h>

#define STIVALE2_MAGIC 0x73747632

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
    Logger::Info(__func__, "Bootloader info at 0x%p", handoff);
    if (magic == STIVALE2_MAGIC) {
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
    Logger::Info(__func__, "Booted via Stivale2");
    // Walk the list of tags in the header
    struct stivale2_struct* fixed = (struct stivale2_struct*)handoff;
    struct stivale2_tag* tag = (struct stivale2_tag*)(fixed->tags);
    while (tag) {
        switch (tag->identifier) {
            case STIVALE2_STRUCT_TAG_MEMMAP_ID: {
                auto memmap = (struct stivale2_struct_tag_memmap*)tag;
                Logger::Debug(__func__, "Found %Lu Stivale2 memmap entries.", memmap->entries);
                if (memmap->entries > that->m_memoryMap.Count()) {
                    panic("Not enough space to add all memory map entries!");
                }
                // Follows the tag list order in stivale2.h
                for (size_t i = 0; i < memmap->entries; i++) {
                    auto entry = memmap->memmap[i];
                    uint64_t end = entry.base + entry.length - 1;
                    that->m_memoryMap[i] = Memory::Section(entry.base, end);
                    Logger::Debug(__func__, "[%zu] 0x%08LX-0x%08LX 0x%08LX", i, entry.base, end, entry.length);
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
                Logger::Debug(__func__, "Stivale2 cmdline: '%s'", that->m_cmdline);
                parseCommandLine(that->m_cmdline);
                break;
            }
            case STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID: {
                auto framebuffer = (struct stivale2_struct_tag_framebuffer*)tag;
                Logger::Debug(
                    __func__,
                    "Stivale2 framebuffer:\n"
                    "\tAddress: 0x%08LX\n"
                    "\tResolution: %ux%ux%u\n"
                    "\tPixel format:\n"
                    "\t\tRed size:    %u\n"
                    "\t\tRed shift:   %u\n"
                    "\t\tGreen size:  %u\n"
                    "\t\tGreen shift: %u\n"
                    "\t\tBlue size:   %u\n"
                    "\t\tBlue shift:  %u",
                    framebuffer->framebuffer_addr,
                    framebuffer->framebuffer_width,
                    framebuffer->framebuffer_height,
                    framebuffer->framebuffer_bpp,
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
                Logger::Debug(__func__, "Unknown Stivale2 tag: 0x%016LX", tag->identifier);
                break;
            }
        }

        tag = (struct stivale2_tag*)tag->next;
    }

    Logger::Debug(__func__, "Done parsing Stivale2 tags");
}

} // !namespace Handoff
