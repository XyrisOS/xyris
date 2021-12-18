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
#include <Arch/i686/Bootloader/EarlyPanic.hpp>
#include <Arch/i686/Bootloader/Loader.hpp>
#include <Arch/i686/Memory.i686.hpp>
#include <Support/sections.hpp>
#include <stdint.h>
#include <stivale/stivale2.h>

#define STIVALE2_MAGIC "stv2"

//-----------------------------------------------
// Stage1 variables
//-----------------------------------------------

// reserve 1024 DWORDs for our page table pointers
__attribute__((section(".early_bss")))
struct Directory pageDirectory;

// lowmem identity mappings
__attribute__((section(".early_bss")))
uint32_t lowMemoryPageTable[1024];

// kernel page table mappings
__attribute__((section(".early_bss")))
uint32_t kernelPageTable[1024];

// page table that maps pages that contain page tables
__attribute__((section(".early_bss")))
uint32_t pagesPageTable[1024];

// page table to hold bootloader structures
__attribute__((section(".early_bss")))
uint32_t bootloaderPageTable[1024];

// stack for C/C++ runtime
__attribute__((section(".early_bss")))
uint32_t stackPageTable[1024];

// TODO: Remove & rename once boot.s is gone
uint32_t stivale2_mmap_helper(struct stivale2_tag* tag);

//-----------------------------------------------
// Stage1 functions
//-----------------------------------------------

static void stage1MapBootloader(void);
static void stage1MapHighMemory(void);
static void stage1MapLowMemory(void);
static void stage1Entry(struct stivale2_struct *info);

/**
 * @brief Scan the stivale2 tags to find the reclaimable bootloader
 * memory map tag that starts at the stivale2 tag base address. Return
 * the length found. We'll memory map the entire area.
 *
 */
uint32_t
__attribute__((section(".early_text")))
stivale2_mmap_helper(struct stivale2_tag* tag)
{
    while (tag) {
        switch (tag->identifier) {
            case STIVALE2_STRUCT_TAG_MEMMAP_ID:
            {
                struct stivale2_struct_tag_memmap* memmap = (struct stivale2_struct_tag_memmap*)tag;
                for (size_t i = 0; i < memmap->entries; i++) {
                    switch (memmap->memmap[i].type)
                    {
                        case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE:
                            if (((uint32_t)memmap->memmap[i].base) == (uint32_t)tag) {
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
        tag = (struct stivale2_tag*)(uint32_t)tag->next;
    }
    // If we get here, there's a problem so we will panic and never return
    EarlyPanic("Error: Cannot detect bootloader info length!");
}


/**
 * @brief Map bootloader information into the higher half of memory
 * so that the kernel can access it and parse it.
 *
 */
__attribute__((section(".early_text")))
static void stage1MapBootloader(void)
{
    // TODO
}


/**
 * @brief Map kernel memory into the higher half of memory
 * See linker.ld for details on where the kernel should be mapped
 *
 */
__attribute__((section(".early_text")))
static void stage1MapHighMemory(void)
{
    // TODO
}

/**
 * @brief identity map from 0x00000000 to _EARLY_KERNEL_END
 * code assumes that the kernel won't be greater than 3MB
 *
 */
__attribute__((section(".early_text")))
static void stage1MapLowMemory(void)
{
    pageDirectory.entries[0].present = 1;
    pageDirectory.entries[0].readWrite = 1;

    for (size_t i = 0; i < _EARLY_KERNEL_END; i++)
    {
        // TODO
    }
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
    if (!info)
    {
        EarlyPanic("Error: Missing bootloader information!");
    }

    // zero the early BSS to start things off well
    for (size_t i = 0; i < _EARLY_BSS_SIZE; i++) {
        ((uint8_t*)_EARLY_BSS_START)[i] = 0;
    }

    stage1MapLowMemory();
    stage1MapHighMemory();
    stage1MapBootloader();
    stage2Entry(info, (uint32_t)STIVALE2_MAGIC);
    EarlyPanic("Error: Execution returned to stage1!");
}

/*
 *   ___ _   _          _     ___   ___ _               _
 *  / __| |_(_)_ ____ _| |___|_  ) / __| |_ _ _ _  _ __| |_ _  _ _ _ ___ ___
 *  \__ \  _| \ V / _` | / -_)/ /  \__ \  _| '_| || / _|  _| || | '_/ -_|_-<
 *  |___/\__|_|\_/\__,_|_\___/___| |___/\__|_|  \_,_\__|\__|\_,_|_| \___/__/
 *
 *  These structures define what features the Stivale2 compliant bootloader
 *  (such as Limine) should provide for the kernel. Because Stivale2 is made
 *  for 64 bit systems first and foremost, we have to create a "patched" version
 *  of the primary Stivale2 header to properly work for 32 bit, i686, systems
 */

struct stivale2_header_i686 {
    uint32_t entry_point;       // Bootloader entry point address
    uint32_t unused_1;          // Unused on i686
    uint32_t stack;             // Stack to be used for Bootloader & early kernel
    uint32_t unused_2;          // Unused on i686
    uint32_t flags;             // Stivale2 flags
    uint32_t unused_3;          // Unused on i686
    uint32_t tags;              // Pointer to next Stivale2 tag (bootloader requests)
    uint32_t unused_4;          // Unused on i686
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
