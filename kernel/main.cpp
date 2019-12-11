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
#include <mem/paging.hpp>
// Multiboot Structure
#include <sys/multiboot.hpp>
// Intel i386 architecture
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
void px_kernel_boot_tone();
extern uint32_t placement_address;
/**
 * @brief Global constructor called from the boot assembly
 * OSDev Wiki takes a different approach to this and does
 * all of this in assembly. You can see that for yourself
 * in the Meaty Skeleton tutorial in the crti.S section.
 */
typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void px_call_constructors() {
    for (constructor* i = &start_ctors; i != &end_ctors; i++) {
        (*i)();
    }
}

/**
 * @brief This is the Panix kernel entry point. This function is called directly from the
 * assembly written in boot.S located in arch/x86/boot.S.
 * @todo Figure out how to use the multiboot header passed in to set up virtual memory
 * and other features.
 */
extern "C" void px_kernel_main(uint32_t kernel_heap, const multiboot_info_t* mb_struct) {
    // Print the splash screen to show we've booted into the kernel properly.
    px_kernel_print_splash();
    px_tty_set_color(Blue, Black);
    // Install the GDT
    px_interrupts_disable();
    px_gdt_install();
    /**
     * @todo Make success and fail conditions for all of these and fix SMBIOS
     */
    //char* smbios_addr = px_get_smbios_addr();
    px_isr_install();           // Interrupt Service Requests
    px_kbd_init();              // Keyboard
    px_rtc_init();              // Real Time Clock
    px_timer_init(1000);        // Programmable Interrupt Timer (1ms)
    // Now that we've initialized our core kernel necessities
    // we can initalize paging.
    // Get our multiboot header info for paging first though.
    // Reference: https://github.com/dipolukarov/osdev/blob/master/main.c
    // uint32_t initrd_location = *((uint32_t*)mb_struct->mods_addr);
	// uint32_t initrd_end	= *(uint32_t*)(mb_struct->mods_addr+4);
	// Dont't trample our module with placement accesses, please!
	placement_address = kernel_heap;
    px_paging_init();
    // Enable interrupts now that we're out of a critical area
    px_interrupts_enable();
    // Print some info to show we did things right
    px_rtc_print();
    px_print_debug("Done.", Success);
    px_kernel_boot_tone();
    while (true) {
        // Keep the kernel alive.
    }
    PANIC("Yikes!\nKernel terminated unexpectedly.");
}

void px_kernel_print_splash() {
    px_clear_tty();
    px_tty_set_color(Yellow, Black);
    px_kprint("Welcome to Panix\n");
    px_kprint("Developed by graduates and undergraduates of Cedarville University.\n");
    px_kprint("Copyright Keeton Feavel et al (c) 2019. All rights reserved.\n\n");
    px_tty_set_color(LightCyan, Black);
    px_kprint("Gloria in te domine, Gloria exultate\n\n");
    px_tty_set_color(White, Black);
}

void px_kernel_boot_tone() {
    // Beep beep!
    px_spkr_beep(1000, 50);
    sleep(100);
    px_spkr_beep(1000, 50);
}