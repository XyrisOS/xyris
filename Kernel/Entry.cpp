/**
 * @file entry.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief The entry point into the Xyris kernel.
 * @version 0.3
 * @date 2019-11-14
 *
 * @copyright Copyright the Xyris Contributors (c) 2019
 *
 */
#include "Logger.hpp"
#include "Panic.hpp"
// System library functions
#include <Library/stdio.hpp>
#include <Library/time.hpp>
#include <Scheduler/tasks.hpp>
// Bootloader
#include <Bootloader/Handoff.hpp>
// Architecture specific code
#include <Arch/Arch.hpp>
// Memory management & paging
#include <Memory/paging.hpp>
// Generic devices
#include <Devices/Clock/rtc.hpp>
#include <Devices/Graphics/console.hpp>
#include <Devices/Graphics/graphics.hpp>
#include <Devices/PCSpeaker/spkr.hpp>
#include <Devices/Serial/rs232.hpp>
// Apps
#include <Applications/primes.hpp>
#include <Applications/spinner.hpp>
// Meta
#include <stdint.h>

#define BUILD_DATE \
    (((__DATE__)[7] - '0') * 1000 + \
    ((__DATE__)[8] - '0') * 100  + \
    ((__DATE__)[9] - '0') * 10   + \
    ((__DATE__)[10] - '0') * 1)

static void printSplash();
static void bootTone();

// TODO: Find a better way of doing this in the future
//       Maybe some sort of way to register driver init
//       functions that can be walked and called?
static void devInit()
{
    RS232::init(RS_232_COM1);
    RTC::init();
}

static void printSplash()
{
    Console::printf(
        "\033[93m"
        "Xyris %s\n"
        "Copyright the Xyris Contributors (c) %i. All rights reserved.\n"
        "Kernel source available at %s.\n"
        "\033[0m",
        VER_NAME,
        BUILD_DATE,
        REPO_URL);
    Console::printf("Commit %s (v%s.%s.%s) built on %s at %s.\n\n", COMMIT, VER_MAJOR, VER_MINOR, VER_PATCH, __DATE__, __TIME__);
}

static void bootTone()
{
    // Beep beep!
    spkr_beep(1000, 50);
    spkr_beep(1000, 50);
}

/**
 *  _  __                 _   ___     _
 * | |/ /___ _ _ _ _  ___| | | __|_ _| |_ _ _ _  _
 * | ' </ -_) '_| ' \/ -_) | | _|| ' \  _| '_| || |
 * |_|\_\___|_| |_||_\___|_| |___|_||_\__|_|  \_, |
 *                                            |__/
 */

/**
 * @brief This is the Xyris kernel entry point. This function is called directly
 * from the assembly written in boot.S located in Arch/i686/boot.S. The pragma
 * allows this function to be declared without needing a former declaration.
 */
void kernelEntry(void* info, uint32_t magic)
{
    Arch::CPU::init();
    Arch::CPU::criticalRegion(devInit);

    Boot::Handoff handoff(info, magic);
    Memory::init(handoff.MemoryMap());
    Graphics::init(handoff.FramebufferInfo());
    tasks_init();

    printSplash();
    Time::TimeDescriptor time;
    Console::printf("UTC: %i/%i/%i %i:%i\n",
        time.getMonth(),
        time.getDay(),
        time.getYear(),
        time.getHour(),
        time.getMinutes());
    Logger::Info(__func__, "%s\n%s\n", Arch::CPU::vendor(), Arch::CPU::model());

    struct task compute, status, spinner;
    tasks_new(Apps::find_primes, &compute, TASK_READY, "prime_compute");
    tasks_new(Apps::show_primes, &status, TASK_READY, "prime_display");
    tasks_new(Apps::spinner, &spinner, TASK_READY, "spinner");
    // Now that we're done make a joyful noise
    bootTone();

    // Keep the kernel task alive.
    tasks_block_current(TASK_PAUSED);
    panic("Kernel terminated unexpectedly!");
}
