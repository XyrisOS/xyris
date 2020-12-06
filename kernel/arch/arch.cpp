/**
 * @file arch.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.3
 * @date 2020-06-01
 * 
 * @copyright Copyright the Panix Contributors (c) 2020
 * 
 */
#include <arch/arch.hpp>
#include <lib/stdio.hpp>

// Function declarations
void px_kernel_print_multiboot_memmap(const multiboot_info_t* mb_struct);

const char* px_exception_descriptions[] = {
    "Divide-By-Zero", "Debugging", "Non-Maskable", "Breakpoint",
    "Overflow", "Out Bound Range", "Invalid Opcode", "Device Not Avbl",
    "Double Fault", "Co-CPU Overrun", "Invalid TSS", "Sgmnt !Present",
    "Seg Fault", "Protection Flt", "Page Fault", "RESERVED",
    "Floating Pnt", "Alignment Check", "Machine Check", "SIMD Flt Pnt",
    "Virtualization", "RESERVED", "RESERVED", "RESERVED",
    "RESERVED", "RESERVED", "RESERVED", "RESERVED",
    "RESERVED", "Security Excptn", "RESERVED", "Triple Fault", "FPU Error"
};

/**
 * @brief 
 * 
 * @param code 
 * @param str 
 * @return int 
 */
static inline void px_arch_cpuid(int flag, unsigned long eax, unsigned long ebx, unsigned long ecx, unsigned long edx)
{
    __asm__ volatile ("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(flag));
}

static inline int px_arch_cpuid(int flag, int regs[4]) {
    // ECX and EDX are swapped in order to make the strings readable
    __asm__ volatile ("cpuid" : "=a"(*regs), "=b"(*(regs+1)), "=c"(*(regs+2)), "=d"(*(regs+3)) : "a"(flag));
    return (int)regs[0];
}

static inline int px_arch_cpuid_vendor(int flag, int regs[4]) {
    // ECX and EDX are swapped in order to make the strings readable
    __asm__ volatile ("cpuid" : "=a"(*regs), "=b"(*(regs+0)), "=d"(*(regs+1)), "=c"(*(regs+2)) : "a"(flag));
    return (int)regs[0];
}

const char* px_cpu_get_vendor() {
    static char vendor[16];
    px_arch_cpuid_vendor(0, (int *)(vendor));
    return vendor;
}

const char* px_cpu_get_model() {
    // The CPU model is broken up across 3 different calls, each using
    // EAX, EBX, ECX, and EDX to store the string, so we basically
    // are appending all 4 register values to this char array each time.
    static char model[48];
    px_arch_cpuid(0x80000002, (int *)(model));
    px_arch_cpuid(0x80000003, (int *)(model+16));
    px_arch_cpuid(0x80000004, (int *)(model+32));
    return model;
}

/**
 * @brief Checks if a Multiboot header has been passed into the kernel
 * by the bootloader. Should be called before calling any other
 * multiboot related functions.
 * 
 * @param mb_struct Multiboot header structure
 */
void px_kernel_check_multiboot(const multiboot_info_t* mb_struct) {
    if (mb_struct == nullptr) {
        px_kprintf("Multiboot info missing!\n");
    }
}

/**
 * @brief Prints out the memory map provided by the Multiboot header
 * passed into the kernel by a Multiboot compliant bootloader
 * 
 * @param mb_struct Multiboot header structure
 */
void px_kernel_print_multiboot_memmap(const multiboot_info_t* mb_struct) {
    // Print out our memory size information if provided
    if (mb_struct->flags & MULTIBOOT_INFO_MEMORY) {
        px_kprintf(
            "Memory Lower: \033[95m0x%08X\n\033[0m"
            "Memory Upper: \033[95m0x%08X\n\033[0m"
            "Total Memory: \033[95m0x%08X\n\033[0m",
            mb_struct->mem_lower,
            mb_struct->mem_upper,
            (mb_struct->mem_lower + mb_struct->mem_upper)
        );
    }
    // Print out our memory map if provided
    if (mb_struct->flags & MULTIBOOT_INFO_MEM_MAP) {
        uint32_t *mem_info_ptr = (uint32_t *)mb_struct->mmap_addr;
        // While there are still entries in the memory map
        while (mem_info_ptr < (uint32_t *)(mb_struct->mmap_addr + mb_struct->mmap_length)) {
            multiboot_memory_map_t *curr = (multiboot_memory_map_t *)mem_info_ptr;
            // If the length of the current map entry is not empty
            if (curr->len > 0) {
                // Print out the memory map information
                px_kprintf("\n[0x%08X-0x%08X] ", curr->addr, (curr->addr + curr->len));
                // Print out if the entry is available or reserved
                curr->type == MULTIBOOT_MEMORY_AVAILABLE ? px_kprintf("Available") : px_kprintf("Reserved");
            } else {
                px_kprintf("\033[91mMissing!\033[0m");
            }
            // Increment the curr pointer to the next entry
            mem_info_ptr += curr->size + sizeof(curr->size);
        }
    }
    px_kprintf("\n\n");
}