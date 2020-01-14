/**
 * @file main.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief The entry point into the Panix kernel. Everything is loaded from here.
 * @version 0.1
 * @date 2019-11-14
 * 
 * @copyright Copyright Keeton Feavel et al (c) 2019
 * 
 */
// System library functions
#include <sys/sys.hpp>
// Memory management & paging
#include <mem/heap.hpp>
#include <mem/paging.hpp>
// Intel i386 architecture
#include <arch/x86/multiboot.hpp>
#include <arch/x86/gdt.hpp>
#include <arch/x86/idt.hpp>
#include <arch/x86/isr.hpp>
#include <arch/x86/timer.hpp>
// Generic devices
#include <devices/smbios/smbios.hpp>
#include <devices/kbd/kbd.hpp>
#include <devices/rtc/rtc.hpp>
#include <devices/spkr/spkr.hpp>

void px_kernel_print_splash();
void px_kernel_check_multiboot(const multiboot_info_t* mb_struct);
void px_kernel_print_multiboot(const multiboot_info_t* mb_struct);
void px_kernel_boot_tone();
extern uint32_t placement_address;
/**
 * @brief The global constuctor is a necessary step when using
 * global objects which need to be constructed before the main
 * function, px_kernel_main() in our case, is ever called. This
 * is much more necessary in an object-oriented architecture,
 * so it is less of a concern now. Regardless, the OSDev Wiki
 * take a *very* different approach to this, so refactoring
 * this might be on the eventual todo list.
 * 
 * According to the OSDev Wiki this is only necessary for C++
 * objects. However, it is useful to know that the
 * global constructors are "stored in a sorted array of 
 * function pointers and invoking these is as simple as 
 * traversing the array and running each element."
 * 
 */
typedef void (*constructor)();
extern "C" constructor _CTORS_START;
extern "C" constructor _CTORS_END;
extern "C" void px_call_constructors() {
    // For each global object with a constructor starting at start_ctors,
    for (constructor* i = &_CTORS_START; i != &_CTORS_END; i++) {
        // Get the object and call the constructor manually.
        (*i)();
    }
}

/**
 * @brief This is the Panix kernel entry point. This function is called directly from the
 * assembly written in boot.S located in arch/x86/boot.S.
 */
extern "C" void px_kernel_main(const multiboot_info_t* mb_struct, uint32_t mb_magic) {
    // Print the splash screen to show we've booted into the kernel properly.
    px_kernel_print_splash();
    // Panix requires a multiboot header, so panic if not provided
    px_kernel_check_multiboot(mb_struct);
    // Passed multiboot check, so continue printing boot info
    px_tty_set_color(Blue, Black);
    // Install the GDT
    px_interrupts_disable();
    px_gdt_install();
    px_isr_install();           // Interrupt Service Requests
    px_heap_init((uint32_t)&_EARLY_KMALLOC_START, (uint32_t)&_EARLY_KMALLOC_END);             // Early kernel memory allocation
    px_paging_init();           // Initialize paging service
    px_kbd_init();              // Keyboard
    px_rtc_init();              // Real Time Clock
    px_timer_init(1000);        // Programmable Interrupt Timer (1ms)
    // Now that we've initialized our core kernel necessities
    // we can initialize paging.
    // Enable interrupts now that we're out of a critical area
    px_interrupts_enable();
    // Print some info to show we did things right
    px_rtc_print();
    px_print_debug("Done.", Success);
    px_kernel_boot_tone();
    while (true) {
        // Keep the kernel alive.
        asm("hlt");
    }
    PANIC("Kernel terminated unexpectedly!");
}

void px_kernel_print_splash() {
    px_clear_tty();
    px_tty_set_color(Yellow, Black);
    px_kprint("Welcome to Panix\n");
    px_kprint("Developed by graduates and undergraduates of Cedarville University.\n");
    px_kprint("Copyright Keeton Feavel et al (c) 2019. All rights reserved.\n");
    px_tty_set_color(White, Black);
    px_kprint("Built on ");
    px_kprint(__DATE__);
    px_kprint(" at ");
    px_kprint(__TIME__);
    px_tty_set_color(LightCyan, Black);
    px_kprint(".\n\n");
    px_tty_set_color(White, Black);
}

void px_kernel_check_multiboot(const multiboot_info_t* mb_struct) {
    if (mb_struct == nullptr) {
        PANIC("Multiboot info missing. Please use a Multiboot compliant bootloader (like GRUB).");
    }
    // Print multiboot information
    px_kernel_print_multiboot(mb_struct);
}

void px_kernel_print_multiboot(const multiboot_info_t* mb_struct) {
    // Print out our memory size information if provided
    if (mb_struct->flags & MULTIBOOT_INFO_MEMORY) {
        uint32_t mem_total = mb_struct->mem_lower + mb_struct->mem_upper;
        px_kprint("Memory Lower: ");
        px_kprint_hex(mb_struct->mem_lower);
        px_kprint("\nMemory Upper: ");
        px_kprint_hex(mb_struct->mem_upper);
        px_kprint_color("\nTotal Memory: ", Magenta);
        px_kprint_hex(mem_total);
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
                px_kprint("\n[");
                px_kprint_hex(curr->addr);
                px_kprint("-");
                px_kprint_hex((curr->addr + curr->len));
                px_kprint("] ");
                // Print out if the entry is available or reserved
                curr->type == MULTIBOOT_MEMORY_AVAILABLE ? px_kprint("Available") : px_kprint("Reserved");
            } else {
                px_kprint_color("Missing!", Red);
            }
            // Increment the curr pointer to the next entry
            mem_info_ptr += curr->size + sizeof(curr->size);
        }
    }
    px_kprint("\n");
}

void px_kernel_boot_tone() {
    // Beep beep!
    px_spkr_beep(1000, 50);
    sleep(100);
    px_spkr_beep(1000, 50);
}