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
#include <lib/stdio.hpp>
#include <lib/string.hpp>
#include <lib/time.hpp>
#include <stdint.h>
#include <sys/panic.hpp>
#include <sys/tasks.hpp>
// Bootloader
#include <boot/Handoff.hpp>
// Memory management & paging
#include <mem/heap.hpp>
#include <mem/paging.hpp>
// Architecture specific code
#include <arch/arch.hpp>
// Generic devices
#include <dev/kbd/kbd.hpp>
#include <dev/rtc/rtc.hpp>
#include <dev/serial/rs232.hpp>
#include <dev/spkr/spkr.hpp>
#include <dev/tty/tty.hpp>
#include <dev/vga/fb.hpp>
#include <dev/vga/graphics.hpp>
// Apps
#include <apps/animation.hpp>
#include <apps/primes.hpp>
#include <apps/spinner.hpp>
// Debug
#include <lib/assert.hpp>
// Meta
#include <meta/defines.hpp>

static Boot::Handoff handoff;
static void kernel_print_splash();
static void kernel_boot_tone();

static void boot_init(void* boot_info, uint32_t magic)
{
    // Parse the bootloader information into common format
    handoff = Boot::Handoff(boot_info, magic);
    // Ensure handoff is no longer default initialized
    assert(handoff.getHandle());
}

/**
 * @brief This is the Panix kernel entry point. This function is called directly from the
 * assembly written in boot.S located in arch/i386/boot.S.
 */
void kernel_main(void* boot_info, uint32_t magic)
{
    // Print the splash screen to show we've booted into the kernel properly.
    kernel_print_splash();
    // Install the GDT
    interrupts_disable();
    gdt_install();               // Initialize the Global Descriptor Table
    isr_install();               // Initialize Interrupt Service Requests
    rs232::init(RS_232_COM1);    // RS232 Serial
    boot_init(boot_info, magic); // Initialize bootloader information
    paging_init(0);              // Initialize paging service (0 is placeholder)
    fb::init(handoff.getFramebufferInfo());
    kbd_init();       // Initialize PS/2 Keyboard
    rtc_init();       // Initialize Real Time Clock
    timer_init(1000); // Programmable Interrupt Timer (1ms)
    // Enable interrupts now that we're out of a critical area
    interrupts_enable();
    // Print some info to show we did things right
    Time::TimeDescriptor time;
    time.printDate();
    // Get the CPU vendor and model data to print
    const char* vendor = cpu_get_vendor();
    const char* model = cpu_get_model();
    kprintf(DBG_INFO "%s %s\n", vendor, model);
    // Print out the CPU vendor info
    rs232::printf("%s\n%s\n", vendor, model);

    tasks_init();
    task_t compute, status, spinner, animation;
    tasks_new(apps::find_primes, &compute, TASK_READY, "prime_compute");
    tasks_new(apps::show_primes, &status, TASK_READY, "prime_display");
    tasks_new(apps::spinner, &spinner, TASK_READY, "spinner");
    tasks_new(apps::testAnimation, &animation, TASK_READY, "testAnimation");
    // Now that we're done make a joyful noise
    kernel_boot_tone();

    // Keep the kernel task alive.
    tasks_block_current(TASK_PAUSED);
    PANIC("Kernel terminated unexpectedly!");
}

static void kernel_print_splash()
{
    tty_clear();
    kprintf(
        "\033[93m"
        "Panix %s\n"
        "Copyright the Panix Contributors (c) %i. All rights reserved.\n"
        "Kernel source available at %s.\n"
        "\033[0m",
        VER_NAME,
        (
            ((__DATE__)[7] - '0') * 1000 + ((__DATE__)[8] - '0') * 100 + ((__DATE__)[9] - '0') * 10 + ((__DATE__)[10] - '0') * 1),
        REPO_URL);
    kprintf("Commit %s (v%s.%s.%s) built on %s at %s.\n\n", COMMIT, VER_MAJOR, VER_MINOR, VER_PATCH, __DATE__, __TIME__);
}

static void kernel_boot_tone()
{
    // Beep beep!
    spkr_beep(1000, 50);
    sleep(100);
    spkr_beep(1000, 50);
}
