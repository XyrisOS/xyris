/**
 * @file main.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief The entry point into the Panix kernel. Everything is loaded from here.
 * @version 0.3
 * @date 2019-11-14
 *
 * @copyright Copyright the Panix Contributors (c) 2019
 *
 */
// System library functions
#include <stdint.h>
#include <sys/panic.hpp>
#include <lib/string.hpp>
#include <lib/stdio.hpp>
// Memory management & paging
#include <mem/heap.hpp>
#include <mem/paging.hpp>
// Architecture specific code
#include <arch/arch.hpp>
// Generic devices
#include <dev/tty/tty.hpp>
#include <dev/kbd/kbd.hpp>
#include <dev/rtc/rtc.hpp>
#include <dev/spkr/spkr.hpp>
#include <dev/serial/rs232.hpp>

// Used as a magic number for stack smashing protection
#if UINT32_MAX == UINTPTR_MAX
#define STACK_CHK_GUARD 0xDEADC0DE
#else
#define STACK_CHK_GUARD 0xBADBADBADBADBAD1
#endif

void px_kernel_print_splash();
void px_kernel_check_multiboot(const multiboot_info_t* mb_struct);
void px_kernel_print_multiboot(const multiboot_info_t* mb_struct);
void px_kernel_boot_tone();

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
 * @brief This function is the global handler for all
 * stack protection. GCC will automatically write the
 * canary code and use this function as the handler
 * for when a smashed stack is detected.
 * 
 */
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;
extern "C" void __stack_chk_fail(void)
{
	PANIC("Smashed stack detected.");
}

/**
 * @brief This is the Panix kernel entry point. This function is called directly from the
 * assembly written in boot.S located in arch/i386/boot.S.
 */
extern "C" void px_kernel_main(const multiboot_info_t* mb_struct, uint32_t mb_magic) {
    // Print the splash screen to show we've booted into the kernel properly.
    px_kernel_print_splash();
    // Panix requires a multiboot header, so panic if not provided
    px_kernel_check_multiboot(mb_struct);
    // Install the GDT
    px_interrupts_disable();
    px_gdt_install();
    px_isr_install();           // Interrupt Service Requests
    // px_heap_init((uint32_t)&_EARLY_KMALLOC_START, (uint32_t)&_EARLY_KMALLOC_END);             // Early kernel memory allocation
    px_paging_init();           // Initialize paging service
    px_kbd_init();              // Keyboard
    px_rtc_init();              // Real Time Clock
    px_timer_init(1000);        // Programmable Interrupt Timer (1ms)
    px_rs232_init(RS_232_COM1); // RS232 Serial
    // Now that we've initialized our core kernel necessities
    // we can initialize paging.
    // Enable interrupts now that we're out of a critical area
    px_interrupts_enable();
    // Print some info to show we did things right
    px_rtc_print();
    // Get the CPU vendor and model data to print
    char *vendor = (char *)px_cpu_get_vendor();
    char *model = (char *)px_cpu_get_model();
    px_kprintf(DBG_INFO "%s\n", vendor);
    px_kprintf(DBG_INFO "%s\n", model);
    // Start the serial debugger
    px_kprintf(DBG_INFO "Starting serial debugger...\n");
    // Print out the CPU vendor info
    px_rs232_print(vendor);
    px_rs232_print("\n");
    px_rs232_print(model);
    px_rs232_print("\n");
    // Now that we're done make a joyful noise
    px_kprintf(DBG_OKAY "Done.\n");
    px_kernel_boot_tone();
    // Keep the kernel alive.
    px_kprintf("\n");
    int i = 0;
    while (true) {
        // Display a spinner to know that we're still running.
        switch (i) {
            case 0:
                px_kprintf("\b|");
                break;
            case 1:
                px_kprintf("\b/");
                break;
            case 2:
                px_kprintf("\b-");
                break;
            case 3:
                px_kprintf("\b\\");
                i = -1;
                break;
        }
        i++;
        asm("hlt");
    }
    PANIC("Kernel terminated unexpectedly!");
}

void px_kernel_print_splash() {
    px_tty_clear();
    px_kprintf(
        "\033[93mWelcome to Panix\n"
        "Developed by graduates and undergraduates of Cedarville University.\n"
        "Copyright Keeton Feavel et al (c) %i. All rights reserved.\n\033[0m",
        (\
            ((__DATE__)[7] - '0') * 1000 + \
            ((__DATE__)[8] - '0') * 100  + \
            ((__DATE__)[9] - '0') * 10   + \
            ((__DATE__)[10] - '0') * 1     \
        )
    );
    px_kprintf("Built on %s at %s.\n\n", __DATE__, __TIME__);
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

void px_kernel_boot_tone() {
    // Beep beep!
    px_spkr_beep(1000, 50);
    sleep(100);
    px_spkr_beep(1000, 50);
}
