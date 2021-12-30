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
#include <Arch/i686/regs.hpp>
#include <Support/sections.hpp>
#include <stdint.h>

// Use the 32-bit version of stivale2
#define _STIVALE2_SPLIT_64
#include <stivale/stivale2.h>

#define PAGE_DIR_ENTRY_SHIFT    22
#define PAGE_TABLE_ENTRY_SHIFT  12
#define KERNEL_STACK_SZ         1024
#define STIVALE2_MAGIC          0x73747632 // "stv2"

//-----------------------------------------------
// Stage1 variables
//-----------------------------------------------

// reserve 1024 DWORDs for our page table pointers
__attribute__((section(".early_bss"),aligned(4096)))
struct Directory pageDirectory;

// lowmem identity mappings
__attribute__((section(".early_bss"),aligned(4096)))
struct Table lowMemoryPageTable;

// kernel page table mappings
__attribute__((section(".early_bss"),aligned(4096)))
struct Table kernelPageTable[2];

// page table to hold bootloader structures
__attribute__((section(".early_bss"),aligned(4096)))
struct Table bootPageTable;

// stack for C/C++ runtime
__attribute__((section(".early_bss"),aligned(4096)))
struct Table stackPageTable;

// bootloader info pointer (for preserving when stack changes)
__attribute__((section(".early_bss")))
struct stivale2_struct *stivale2Info;

// stack for stage1 C/C++ runtime
__attribute__((section(".early_bss")))
uint8_t stage1Stack[KERNEL_STACK_SZ];

// stack for stage2 C/C++ runtime
__attribute__((section(".bss")))
uint8_t stage2Stack[KERNEL_STACK_SZ];

//-----------------------------------------------
// Stage1 functions
//-----------------------------------------------

static void stage1MapBootloader(void);
static void stage1MapHighMemory(void);
static void stage1MapLowMemory(void);
static void stage1Entry(struct stivale2_struct *info);
static uint32_t stage1GetBootInfoAddr(struct stivale2_tag* tag);

/**
 * @brief Scan the stivale2 tags to find the reclaimable bootloader
 * memory map tag that starts at the stivale2 tag base address. Return
 * the length found. We'll memory map the entire area.
 *
 */
__attribute__((section(".early_text")))
static uint32_t stage1GetBootInfoAddr(struct stivale2_tag* tag)
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
 * @brief Finalize stage1 by loading the finalized page directory
 * and enabling paging.
 *
 */
__attribute__((section(".early_text")))
static void stage1EnablePaging(void)
{
    setPageDirectory((uintptr_t)&pageDirectory);

    struct CR0 cr0 = readCR0();
    cr0.pagingEnable = 1;
    writeCR0(cr0);
}

/**
 * @brief Prepare a higher-half stack for kernel usage.
 *
 */
__attribute__((section(".early_text")))
static void stage1JumpToStage2(void)
{
    // zero the kernel BSS (higher half stack)
    for (size_t i = 0; i < (size_t)&_BSS_SIZE; i++) {
        ((uint8_t*)&_BSS_START)[i] = 0;
    }

    // adjust the stack pointer to use the higher half, kernel stack
    asm volatile (
        "mov %0, %%esp"
        : // no output
        : "r" ((stage2Stack + sizeof(stage2Stack)))
    );

    // Jump to stage2 before returning
    stage2Entry(stivale2Info, STIVALE2_MAGIC);
    EarlyPanic("Error: Failed to make the jump to stage2!");
}

/**
 * @brief Map bootloader information into the higher half of memory
 * so that the kernel can access it and parse it.
 *
 */
__attribute__((section(".early_text")))
static void stage1MapBootloader(void)
{
    // Grab the address of the stivale2 boot info tags
    uint32_t stivale2InfoAddr = (uint32_t)stivale2Info->tags;

    // Bootloader info page directory
    uint32_t bootDirectoryEntryIdx = stivale2InfoAddr >> PAGE_DIR_ENTRY_SHIFT;
    struct DirectoryEntry* bootDirectoryEntry = &pageDirectory.entries[bootDirectoryEntryIdx];

    // Determine if page table needs to be initalized
    struct Table *bootTable;
    if (bootDirectoryEntry->present) {
        bootTable = (struct Table*)(bootDirectoryEntry->tableAddr << PAGE_TABLE_ENTRY_SHIFT);
    } else {
        bootDirectoryEntry->present = 1;
        bootDirectoryEntry->readWrite = 1;
        bootDirectoryEntry->tableAddr = (uint32_t)&bootPageTable >> PAGE_TABLE_ENTRY_SHIFT;
        bootTable = &bootPageTable;
    }

    // Get the length of the bootloader information
    struct stivale2_tag *tags = (struct stivale2_tag *)(uintptr_t)stivale2Info->tags;
    uint32_t stivale2InfoLength = stage1GetBootInfoAddr(tags);
    uint32_t stivale2InfoEnd = stivale2InfoAddr + stivale2InfoLength;

    // Map in the entire bootloader information linked list
    for (uintptr_t addr = stivale2InfoAddr; addr < stivale2InfoEnd; addr += ARCH_PAGE_SIZE) {
        size_t bootMemoryIdx = addr >> PAGE_TABLE_ENTRY_SHIFT;
        struct TableEntry* bootTableEntry = &bootTable->pages[bootMemoryIdx & 0xfff];
        bootTableEntry->present = 1;
        bootTableEntry->readWrite = 1;
        bootTableEntry->frame = bootMemoryIdx;
    }
}


/**
 * @brief Map kernel memory into the higher half of memory
 * See linker.ld for details on where the kernel should be mapped
 *
 */
__attribute__((section(".early_text")))
static void stage1MapHighMemory(void)
{
    uint32_t kernelDirectoryEntryIdx = (uint32_t)&_KERNEL_START >> PAGE_DIR_ENTRY_SHIFT;

    // First kernel page table entry
    struct DirectoryEntry* kernelDirectoryEntry = &pageDirectory.entries[kernelDirectoryEntryIdx];
    kernelDirectoryEntry->present = 1;
    kernelDirectoryEntry->readWrite = 1;
    kernelDirectoryEntry->tableAddr = (uint32_t)&kernelPageTable[0] >> PAGE_TABLE_ENTRY_SHIFT;

    // Second kernel page table entry
    struct DirectoryEntry* pagesDirectoryEntry = kernelDirectoryEntry + 1;
    pagesDirectoryEntry->present = 1;
    pagesDirectoryEntry->readWrite = 1;
    pagesDirectoryEntry->tableAddr = (uint32_t)&kernelPageTable[1] >> PAGE_TABLE_ENTRY_SHIFT;

    for (uintptr_t addr = KERNEL_START; addr < KERNEL_END; addr += ARCH_PAGE_SIZE) {
        size_t kernelMemoryIdx = addr >> PAGE_TABLE_ENTRY_SHIFT;
        struct TableEntry* kernelMemoryTableEntry = &kernelPageTable[0].pages[kernelMemoryIdx & 0x1fff];
        kernelMemoryTableEntry->present = 1;
        kernelMemoryTableEntry->readWrite = 1;
        kernelMemoryTableEntry->frame = kernelMemoryIdx - 0xc0000;
    }
}

/**
 * @brief identity map from 0x00000000 -> LOWMEM_END
 *
 */
__attribute__((section(".early_text")))
static void stage1MapLowMemory(void)
{
    // WARNING: code assumes that the kernel won't be greater than 3MB
    struct DirectoryEntry* lowMem = &pageDirectory.entries[0];
    lowMem->present = 1;
    lowMem->readWrite = 1;
    lowMem->tableAddr = (uint32_t)&lowMemoryPageTable >> PAGE_TABLE_ENTRY_SHIFT;

    // Map in the entirety of low-memory and stage1
    for (uintptr_t addr = ARCH_PAGE_SIZE; addr < EARLY_KERNEL_END; addr += ARCH_PAGE_SIZE) {
        size_t pageIdx = addr >> PAGE_TABLE_ENTRY_SHIFT;
        struct TableEntry* lowMemoryTableEntry = &lowMemoryPageTable.pages[pageIdx & 0xfff];
        lowMemoryTableEntry->present = 1;
        lowMemoryTableEntry->readWrite = 1;
        lowMemoryTableEntry->frame = pageIdx;
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
    // By this point the stivale2 bootloader should have provided us a healthy
    // stack to use (at the location we specified in stivale2_header_i686 struct)
    if (!info) {
        // xyris requires information (such as memory maps, framebuffer location, etc)
        // from the bootloader, so if that's missing we're already in trouble.
        EarlyPanic("Error: Missing bootloader information!");
    }

    // preserve bootloader information pointer so that when we move the
    // stack pointer we don't lose the one thing we need to pass into
    // stage2 / the higher half stack
    stivale2Info = info;

    stage1MapLowMemory();
    stage1MapHighMemory();
    stage1MapBootloader();
    stage1EnablePaging();
    stage1JumpToStage2();
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

__attribute__((section(".early_data"), used))
const struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = 0,
    },
    .framebuffer_width  = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp    = 0,
};

__attribute__((section(".stivale2hdr"), used))
const struct stivale2_header stivale_hdr = {
    .entry_point = (uint32_t)stage1Entry,
    .stack = (uint32_t)stage1Stack + sizeof(stage1Stack),
    .flags = 0,
    .tags = (uint32_t)&framebuffer_hdr_tag,
};
