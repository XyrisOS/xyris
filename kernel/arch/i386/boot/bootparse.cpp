#include <stdint.h>
#include <lib/stdio.hpp>
#include <lib/string.hpp>
#include <mem/paging.hpp>
#include <dev/serial/rs232.hpp>

#include <multiboot/multiboot2.h>
#include <stivale/stivale2.h>

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
        px_rs232_printf("  addr: 0x%02x%08x, length: 0x%02x%08x, type: %s\n",
            (uint32_t)(entry->addr >> 32) & 0xff, (uint32_t)(entry->addr & UINT32_MAX),
            (uint32_t)(entry->len >> 32) & 0xff, (uint32_t)(entry->len & UINT32_MAX),
            mmap_types[entry->type]);
        entry = (struct multiboot_mmap_entry *)((uintptr_t)entry + mmap->entry_size);
        remaining -= mmap->entry_size;
    }
}

struct RSDPDescriptor {
    char Signature[8];
    uint8_t Checksum;
    char OEMID[6];
    uint8_t Revision;
    uint32_t RsdtAddress;
} __attribute__ ((packed));

static void print_acpi1_rsdp(struct multiboot_tag_old_acpi *acpi)
{
    auto rsdp = (struct RSDPDescriptor *) acpi->rsdp;
    size_t checksum = 0;
    for (unsigned i = 0; i < sizeof(*rsdp); i++) {
        checksum += ((uint8_t*)rsdp)[i];
    }
    bool is_valid = (uint8_t)checksum == 0 && memcmp(rsdp->Signature, "RSD PTR ", sizeof(rsdp->Signature)) == 0;
    px_rs232_printf("Multiboot2 ACPI 1.0 RSDP:\n");
    px_rs232_printf("  Checksum: %s\n", is_valid ? "Valid" : "Invalid");
    px_rs232_printf("  OEMID: %.6s\n", rsdp->OEMID);
    px_rs232_printf("  Revision: %u\n", rsdp->Revision);
    px_rs232_printf("  RsdtAddress: 0x%08x\n", rsdp->RsdtAddress);
}

void px_parse_multiboot2(void *info)
{
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
            print_acpi1_rsdp(oldacpi);
            break;
        }
        case MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR: {
            auto loadbase = (struct multiboot_tag_load_base_addr *) tag;
            px_rs232_printf("Multiboot2 base load address: 0x%p\n", loadbase->load_base_addr);
            break;
        }
        default:
            px_rs232_printf("Unknown Multiboot2 Tag: %d\n", tag->type);
            break;
        }
        // move to the next tag, aligning if necessary
        tag = (struct multiboot_tag*)(((uintptr_t)tag + tag->size + 7) & ~((uintptr_t)7));
    }
}
