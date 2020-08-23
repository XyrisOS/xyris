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
// Define the Git commit version if not declared by compiler
#ifndef VERSION
#define VERSION "unknown"
#endif

void px_kernel_print_splash();
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
    // Install the GDT
    px_interrupts_disable();
    px_gdt_install();           // Initialize the Global Descriptor Table
    px_isr_install();           // Initialize Interrupt Service Requests
    px_rs232_init(RS_232_COM1); // RS232 Serial
    px_paging_init(0);          // Initialize paging service
    px_kbd_init();              // Initialize PS/2 Keyboard
    px_rtc_init();              // Initialize Real Time Clock
    px_timer_init(1000);        // Programmable Interrupt Timer (1ms)
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

    px_kprintf(DBG_WARN "Mapping in test pages\n");
    px_rs232_print("==== MAP TEST PAGES ====\n");
    
    char test_str[] ="this is a test. please do not panic.";
    char *pages[32];
    int32_t i;
    for (i = 0; i < 1024; i++) {
        pages[i] = (char *)px_get_new_page(0);
        if (pages[i] == NULL) {
            px_kprintf(DBG_FAIL "Failed to map in new page at %p\n", pages[i]);
            break;
        }
        memcpy(pages[i], test_str, sizeof(test_str));
    }
 
    for (i -= 1; i >= 0; i--) {
        px_free_page(pages[i], 1);
    }

    px_kprintf(DBG_OKAY "Done.\n");
    
    // Now that we're done make a joyful noise
    px_kernel_boot_tone();
    
    // Keep the kernel alive.
    px_kprintf("\n");
    i = 0;
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
    px_kprintf("Commit %s built on %s at %s.\n\n", VERSION, __DATE__, __TIME__);
}

void px_kernel_boot_tone() {
    // Beep beep!
    px_spkr_beep(1000, 50);
    sleep(100);
    px_spkr_beep(1000, 50);
}
