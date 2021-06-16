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
#include <sys/kernel.hpp>
#include <sys/panic.hpp>
#include <sys/tasks.hpp>
#include <lib/string.hpp>
#include <lib/stdio.hpp>
// Bootloader
#include <boot/Handoff.hpp>
// Memory management & paging
#include <mem/heap.hpp>
#include <mem/paging.hpp>
// Architecture specific code
#include <arch/arch.hpp>
// Generic devices
#include <dev/vga/fb.hpp>
#include <dev/vga/graphics.hpp>
#include <dev/tty/tty.hpp>
#include <dev/kbd/kbd.hpp>
#include <dev/rtc/rtc.hpp>
#include <dev/spkr/spkr.hpp>
#include <dev/serial/rs232.hpp>
// Apps
#include <apps/primes.hpp>
#include <apps/spinner.hpp>
// Debug
#include <lib/assert.hpp>

static Boot::Handoff handoff;

static void kernel_print_splash();
static void kernel_boot_tone();

static void boot_init(void *boot_info, uint32_t magic)
{
    // Map in bootloader information
    // TODO: Find a way to avoid this until after parsing
    if (boot_info != NULL) {
        uintptr_t page = (uintptr_t)boot_info & PAGE_ALIGN;
        map_kernel_page(VADDR(page), page);
    }
    // Parse the bootloader information into common format
    handoff = Boot::Handoff(boot_info, magic);
    // Ensure handoff is no longer default initialized
    assert(handoff.getHandle());
}

/**
 * @brief This is the Panix kernel entry point. This function is called directly from the
 * assembly written in boot.S located in arch/i386/boot.S.
 */
void kernel_main(void *boot_info, uint32_t magic) {
    // Print the splash screen to show we've booted into the kernel properly.
    kernel_print_splash();
    // Install the GDT
    interrupts_disable();
    gdt_install();                  // Initialize the Global Descriptor Table
    isr_install();                  // Initialize Interrupt Service Requests
    rs232_init(RS_232_COM1);        // RS232 Serial
    paging_init(0);                 // Initialize paging service (0 is placeholder)
    boot_init(boot_info, magic);    // Initialize bootloader information
                                    // TODO: Bootloader should be first but currently
                                    //       requires paging, which should come after
                                    //       boot information is parsed.
    fb::init(handoff.getFramebufferInfo());
    kbd_init();                     // Initialize PS/2 Keyboard
    rtc_init();                     // Initialize Real Time Clock
    timer_init(1000);               // Programmable Interrupt Timer (1ms)
    // Enable interrupts now that we're out of a critical area
    interrupts_enable();
    // Enable serial input
    rs232_init_buffer(1024);
    // Print some info to show we did things right
    rtc_print();
    // Get the CPU vendor and model data to print
    const char *vendor = cpu_get_vendor();
    const char *model = cpu_get_model();
    kprintf(DBG_INFO "%s %s\n", vendor, model);
    // Print out the CPU vendor info
    rs232_printf("%s\n%s\n", vendor, model);

    tasks_init();
    task_t compute, status, spinner;
    tasks_new(apps::find_primes, &compute, TASK_READY, "prime_compute");
    tasks_new(apps::show_primes, &status, TASK_READY, "prime_display");
    tasks_new(apps::spinner, &spinner, TASK_READY, "spinner");

    // Now that we're done make a joyful noise
    kernel_boot_tone();

    // Keep the kernel task alive.
    tasks_block_current(TASK_PAUSED);
    PANIC("Kernel terminated unexpectedly!");
}

static void kernel_print_splash() {
    tty_clear();
    kprintf(
        "\033[93mWelcome to Panix %s\n"
        "Developed by graduates and undergraduates of Cedarville University.\n"
        "Copyright the Panix Contributors (c) %i. All rights reserved.\n\033[0m",
        VER_NAME,
        (
            ((__DATE__)[7] - '0') * 1000 + \
            ((__DATE__)[8] - '0') * 100  + \
            ((__DATE__)[9] - '0') * 10   + \
            ((__DATE__)[10] - '0') * 1     \
        )
    );
    kprintf("Commit %s (v%s.%s.%s) built on %s at %s.\n\n", COMMIT, VER_MAJOR, VER_MINOR, VER_PATCH, __DATE__, __TIME__);
}

static void kernel_boot_tone() {
    // Beep beep!
    spkr_beep(1000, 50);
    sleep(100);
    spkr_beep(1000, 50);
}
