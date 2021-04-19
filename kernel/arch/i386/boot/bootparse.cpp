#include <stdint.h>
#include <lib/stdio.hpp>
#include <lib/string.hpp>
#include <mem/paging.hpp>
#include <arch/BootInfo.hpp>
#include <dev/serial/rs232.hpp>

#include <multiboot/multiboot2.h>
#include <stivale/stivale2.h>

/*
 *  __  __      _ _   _ _              _   ___
 * |  \/  |_  _| | |_(_) |__  ___  ___| |_|_  )
 * | |\/| | || | |  _| | '_ \/ _ \/ _ \  _|/ /
 * |_|  |_|\_,_|_|\__|_|_.__/\___/\___/\__/___|
 */

struct multiboot_fixed
{
    uint32_t total_size;
    uint32_t reserved;
};

static void print_multiboot2_mmap(struct multiboot_tag_mmap *mmap)
{
    static const char *mmap_types[] = {
        [0] = "Invalid",
        [MULTIBOOT_MEMORY_AVAILABLE] = "Available",
        [MULTIBOOT_MEMORY_RESERVED] = "Reserved",
        [MULTIBOOT_MEMORY_ACPI_RECLAIMABLE] = "ACPI reclaimable",
        [MULTIBOOT_MEMORY_NVS] = "Non-volatile storage",
        [MULTIBOOT_MEMORY_BADRAM] = "Bad RAM"
    };
    uint32_t remaining = mmap->size - sizeof(*mmap);
    struct multiboot_mmap_entry *entry = mmap->entries;
    while (remaining > 0) {
        px_rs232_printf("\taddr: 0x%02x%08x, length: 0x%02x%08x, type: %s\n",
            (uint32_t)(entry->addr >> 32) & 0xff, (uint32_t)(entry->addr & UINT32_MAX),
            (uint32_t)(entry->len >> 32) & 0xff, (uint32_t)(entry->len & UINT32_MAX),
            mmap_types[entry->type]);
        entry = (struct multiboot_mmap_entry *)((uintptr_t)entry + mmap->entry_size);
        remaining -= mmap->entry_size;
    }
}

static void print_acpi1_rsdp(struct RSDPDescriptor* rsdp)
{
    size_t checksum = 0;
    for (unsigned i = 0; i < sizeof(*rsdp); i++) {
        checksum += ((uint8_t*)rsdp)[i];
    }
    bool is_valid = (uint8_t)checksum == 0 && memcmp(rsdp->Signature, "RSD PTR ", sizeof(rsdp->Signature)) == 0;
    px_rs232_printf("ACPI 1.0 RSDP:\n");
    px_rs232_printf("\tChecksum: %s\n", is_valid ? "Valid" : "Invalid");
    px_rs232_printf("\tOEMID: %.6s\n", rsdp->OEMID);
    px_rs232_printf("\tRevision: %u\n", rsdp->Revision);
    px_rs232_printf("\tRsdtAddress: 0x%08x\n", rsdp->RsdtAddress);
}

void px_parse_multiboot2(BootInfo* handle, void *info)
{
    (void)handle;
    auto fixed = (struct multiboot_fixed *) info;
    for (uintptr_t page = ((uintptr_t)info & PAGE_ALIGN) + PAGE_SIZE;
         page <= (((uintptr_t)info + fixed->total_size) & PAGE_ALIGN);
         page += PAGE_SIZE) {
        px_rs232_printf("Mapping bootinfo at 0x%08x\n", page);
        px_map_kernel_page(VADDR(page), page);
    }
    struct multiboot_tag *tag = (struct multiboot_tag*)((uintptr_t)fixed + sizeof(struct multiboot_fixed));
    while (tag->type != MULTIBOOT_TAG_TYPE_END) {
        switch (tag->type)
        {
        case MULTIBOOT_TAG_TYPE_CMDLINE: {
            auto cmdline = (struct multiboot_tag_string *) tag;
            px_rs232_printf("Multiboot2 cmdline: '%s'\n", cmdline->string);
            break;
        }
        case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME: {
            auto loader_tag = (struct multiboot_tag_string *) tag;
            px_rs232_printf("Multiboot2 bootloader name: %s\n", loader_tag->string);
            break;
        }
        case MULTIBOOT_TAG_TYPE_MODULE: {
            auto module = (struct multiboot_tag_module *) tag;
            px_rs232_printf("Multiboot2 module: %s\n  Module start: 0x%08x\n  Module end:   0x%08x\n",
                module->cmdline, module->mod_start, module->mod_end);
            break;
        }
        case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO: {
            auto meminfo_tag = (struct multiboot_tag_basic_meminfo *) tag;
            px_rs232_printf("Multiboot2 basic meminfo:\n  Lower mem: 0x%08x\n  Upper mem: 0x%08x\n",
                meminfo_tag->mem_lower, meminfo_tag->mem_upper);
            break;
        }
        case MULTIBOOT_TAG_TYPE_BOOTDEV: {
            auto bootdev = (struct multiboot_tag_bootdev *) tag;
            px_rs232_printf("Multiboot2 BIOS boot device:\n  disk: %02x, partition: %d, sub_partition: %d\n",
                bootdev->biosdev, bootdev->part, bootdev->slice);
            break;
        }
        case MULTIBOOT_TAG_TYPE_MMAP: {
            auto mmap = (struct multiboot_tag_mmap *) tag;
            px_rs232_printf("Multiboot2 memory map: version = %d\n", mmap->entry_version);
            print_multiboot2_mmap(mmap);
            break;
        }
        case MULTIBOOT_TAG_TYPE_ACPI_OLD: {
            auto oldacpi = (struct multiboot_tag_old_acpi *) tag;
            print_acpi1_rsdp((struct RSDPDescriptor *)oldacpi->rsdp);
            break;
        }
        case MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR: {
            auto loadbase = (struct multiboot_tag_load_base_addr *) tag;
            px_rs232_printf("Multiboot2 base load address: 0x%p\n", loadbase->load_base_addr);
            break;
        }
        default:
            px_rs232_printf("Unknown Multiboot2 tag: %d\n", tag->type);
            break;
        }
        // move to the next tag, aligning if necessary
        tag = (struct multiboot_tag*)(((uintptr_t)tag + tag->size + 7) & ~((uintptr_t)7));
    }
}

/*
 *  ___ _   _          _     ___
 * / __| |_(_)_ ____ _| |___|_  )
 * \__ \  _| \ V / _` | / -_)/ /
 * |___/\__|_|\_/\__,_|_\___/___|
 */

static const char* stivale2_mmap_type_to_string(uint32_t type)
{
    switch (type)
    {
        case STIVALE2_MMAP_USABLE:
            return "Available";
        case STIVALE2_MMAP_RESERVED:
            return "Reserved";
        case STIVALE2_MMAP_ACPI_RECLAIMABLE:
            return "ACPI reclaimable";
        case STIVALE2_MMAP_ACPI_NVS:
            return "Non-volatile storage";
        case STIVALE2_MMAP_BAD_MEMORY:
            return "Bad RAM";
        case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE:
            return "Bootloader";
        case STIVALE2_MMAP_KERNEL_AND_MODULES:
            return "Kernel & Modules";
        default:
            return "Invalid";
    }
}

static void print_stivale2_mmap(struct stivale2_struct_tag_memmap* mmap)
{
    // List of memory map entries
    auto mmap_list = (struct stivale2_mmap_entry*)mmap->memmap;
    for (uint64_t i = 0; i < mmap->entries; i++)
    {
        auto entry = &mmap_list[i];
        px_rs232_printf("\taddr: 0x%02x%08x, length: 0x%02x%08x, type: %s\n",
            (uint32_t)(entry->base >> 32) & 0xff, (uint32_t)(entry->base & UINT32_MAX),
            (uint32_t)(entry->length >> 32) & 0xff, (uint32_t)(entry->length & UINT32_MAX),
            stivale2_mmap_type_to_string(entry->type)
        );
    }
}

void px_parse_stivale2(BootInfo* handle, void *info)
{
    (void)handle;
    auto fixed = (struct stivale2_struct*)info;
    // Walk the list of tags in the header
    auto tag = (struct stivale2_tag*)(fixed->tags);
    while (tag)
    {
        // Map in each tag since Stivale2 doesn't give us a total size like Multiboot does.
        uintptr_t page = ((uintptr_t)tag & PAGE_ALIGN);
        px_map_kernel_page(VADDR(page), page);
        // Follows the tag list order in stivale2.h
        switch(tag->identifier)
        {
            case STIVALE2_STRUCT_TAG_CMDLINE_ID:
            {
                auto cmdline = (struct stivale2_struct_tag_cmdline*)tag;
                px_rs232_printf("Stivale2 cmdline: '%s'\n", (const char *)cmdline->cmdline);
                break;
            }
            case STIVALE2_STRUCT_TAG_MEMMAP_ID:
            {
                auto memmap = (struct stivale2_struct_tag_memmap*)tag;
                px_rs232_printf("Stivale2 memory map:\n");
                print_stivale2_mmap(memmap);
                break;
            }
            case STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID:
            {
                auto framebuffer = (struct stivale2_struct_tag_framebuffer*)tag;
                px_rs232_printf("Stivale2 framebuffer:\n");
                px_rs232_printf("\tAddress: 0x%08X", framebuffer->framebuffer_addr);
                px_rs232_printf("\tResolution: %ix%ix%i\n",
                    framebuffer->framebuffer_width,
                    framebuffer->framebuffer_height,
                    (framebuffer->framebuffer_bpp * 8));
                break;
            }
            case STIVALE2_STRUCT_TAG_FB_MTRR_ID:
            {
                px_rs232_printf("\tFramebuffer has MTRR\n");
                break;
            }
            case STIVALE2_STRUCT_TAG_MODULES_ID:
            {
                auto modules = (struct stivale2_struct_tag_modules*)tag;
                for (uint64_t i = 0; i < modules->module_count; i++) {
                    stivale2_module* mod = &modules->modules[i];
                    px_rs232_printf("Stivale2 module: %s\n  Module start: 0x%08x\n  Module end:   0x%08x\n",
                        mod->string,
                        mod->begin,
                        mod->end
                    );
                }
                break;
            }
            case STIVALE2_STRUCT_TAG_RSDP_ID:
            {
                auto rsdp = (struct stivale2_struct_tag_rsdp*)tag;
                struct RSDPDescriptor* rsdp_desc = (struct RSDPDescriptor*)rsdp->rsdp;
                print_acpi1_rsdp(rsdp_desc);
                break;
            }
            case STIVALE2_STRUCT_TAG_EPOCH_ID:
            {
                auto epoch = (struct stivale2_struct_tag_epoch*)tag;
                px_rs232_printf("Stivale2 epoch: %i\n", epoch->epoch);
                break;
            }
            case STIVALE2_STRUCT_TAG_FIRMWARE_ID:
            {
                auto firmware = (struct stivale2_struct_tag_firmware*)tag;
                px_rs232_printf("Stivale2 firmware flags: 0x%08X\n", firmware->flags);
                px_rs232_printf("\tBooted using %s\n", (firmware->flags & 0x1 ? "BIOS" : "UEFI"));
                break;
            }
            case STIVALE2_STRUCT_TAG_SMP_ID:
            {
                auto smp = (struct stivale2_struct_tag_smp*)tag;
                px_rs232_printf("Stivale2 SMP flags: 0x%08X\n", smp->flags);
                px_rs232_printf("\tx2APIC %savailable\n", (smp->flags & 0x1 ? "" : "un"));
                px_rs232_printf("\tLAPIC ID: 0x%08X\n", smp->bsp_lapic_id);
                px_rs232_printf("\tCPU Count: %i\n", smp->cpu_count);
                for (uint64_t i = 0; i < smp->cpu_count; i++) {
                    auto smp_info = smp->smp_info[i];
                    px_rs232_printf("\t\tCPU ID: 0x%08X\n", smp_info.processor_id);
                    px_rs232_printf("\t\t\tLAPIC ID: 0x%08X\n", smp_info.lapic_id);
                    px_rs232_printf("\t\t\tStack addr: 0x%08X\n", smp_info.target_stack);
                    px_rs232_printf("\t\t\tgoto addr: 0x%08X\n", smp_info.goto_address);
                    px_rs232_printf("\t\t\textra args: 0x%08X\n", smp_info.extra_argument);
                }
                break;
            }
            case STIVALE2_STRUCT_TAG_PXE_SERVER_INFO:
            {
                auto pxe = (struct stivale2_struct_tag_pxe_server_info*)tag;
                uint8_t ip[4];
                ip[0] = (uint8_t)(pxe->server_ip & 0xFF);
                ip[1] = (uint8_t)((pxe->server_ip >> 8) & 0xFF);
                ip[2] = (uint8_t)((pxe->server_ip >> 16) & 0xFF);
                ip[3] = (uint8_t)((pxe->server_ip >> 24) & 0xFF);
                px_rs232_printf("Stivale2 PXE ip addr: %d.%d.%d.%d\n",
                    ip[3], ip[2], ip[1], ip[0]
                );
                break;
            }
            default:
            {
                px_rs232_printf("Unknown Stivale2 tag: %d\n", tag->identifier);
                break;
            }
        }

        tag = (struct stivale2_tag*)tag->next;
    }
    px_rs232_printf("Done\n");
}
