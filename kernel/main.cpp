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
#include <sys/tasks.hpp>
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
// Apps
#include <apps/primes.hpp>

// Used as a magic number for stack smashing protection
#if UINT32_MAX == UINTPTR_MAX
    #define STACK_CHK_GUARD 0xDEADC0DE
#else
    #define STACK_CHK_GUARD 0xBADBADBADBADBAD1
#endif
// Define the Git commit version if not declared by compiler
#ifndef COMMIT
    #define COMMIT "unknown"
#endif
// Assume that if major isn't set none of them are
#ifndef VER_MAJOR
    #define VER_MAJOR '0'
    #define VER_MINOR '0'
    #define VER_PATCH '0'
#endif

extern "C" void __stack_chk_fail(void);
extern "C" void px_kernel_main(const multiboot_info_t* mb_struct, uint32_t mb_magic);
void _px_kernel_print_splash();
void _px_kernel_boot_tone();

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
    (void)mb_struct;
    (void)mb_magic;
    // Print the splash screen to show we've booted into the kernel properly.
    _px_kernel_print_splash();
    // Install the GDT
    px_interrupts_disable();
    px_gdt_install();           // Initialize the Global Descriptor Table
    px_isr_install();           // Initialize Interrupt Service Requests
    px_rs232_init(RS_232_COM1); // RS232 Serial
    px_paging_init(0);          // Initialize paging service (0 is placeholder)
    px_kbd_init();              // Initialize PS/2 Keyboard
    px_rtc_init();              // Initialize Real Time Clock
    px_timer_init(1000);        // Programmable Interrupt Timer (1ms)
    // Enable interrupts now that we're out of a critical area
    px_interrupts_enable();
    // Enable serial input
    px_rs232_init_buffer(1024);
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
    // Done
    px_kprintf(DBG_OKAY "Done.\n");

    px_tasks_init();
    px_task_t compute, status;
    px_tasks_new(find_primes, &compute, TASK_READY, "prime_compute");
    px_tasks_new(show_primes, &status, TASK_READY, "prime_display");

    // Now that we're done make a joyful noise
    _px_kernel_boot_tone();

    // Keep the kernel alive.
    px_kprintf("\n");
    int i = 0;
    const char spinnay[] = { '|', '/', '-', '\\' };
    while (true) {
        // Display a spinner to know that we're still running.
        px_kprintf("\e[s\e[24;0f%c\e[u", spinnay[i]);
        i = (i + 1) % sizeof(spinnay);
        asm volatile("hlt");
    }
    PANIC("Kernel terminated unexpectedly!");
}

void _px_kernel_print_splash() {
    px_tty_clear();
    px_kprintf(
        "\033[93mWelcome to Panix\n"
        "Developed by graduates and undergraduates of Cedarville University.\n"
        "Copyright the Panix Contributors (c) %i. All rights reserved.\n\033[0m",
        (\
            ((__DATE__)[7] - '0') * 1000 + \
            ((__DATE__)[8] - '0') * 100  + \
            ((__DATE__)[9] - '0') * 10   + \
            ((__DATE__)[10] - '0') * 1     \
        )
    );
    px_kprintf("Commit %s (v%s.%s.%s) built on %s at %s.\n\n", COMMIT, VER_MAJOR, VER_MINOR, VER_PATCH, __DATE__, __TIME__);
}

void _px_kernel_boot_tone() {
    // Beep beep!
    px_spkr_beep(1000, 50);
    sleep(100);
    px_spkr_beep(1000, 50);
}
