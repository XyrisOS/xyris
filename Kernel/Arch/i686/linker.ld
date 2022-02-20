/**
 * @file linker.ld
 * @author Micah Switzer (mswitzer@cedarville.edu)
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief i386 kernel linker script
 * @version 0.1
 * @date 2021-06-27
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)

SECTIONS
{
    /* Early kernel section, linked at 1MB */
    _EARLY_MEM_START = .;
    . = 0x00100000;
    _EARLY_KERNEL_START = .;
    .stivale2hdr : { *(.stivale2hdr) }
    .early_text ALIGN (4K) : { *(.early_text) }
    .early_data ALIGN (4K) : { *(.early_data) }
    .early_bss ALIGN (4K) :
    {
        _EARLY_BSS_START = .;
        *(.early_bss)
        _EARLY_BSS_END = .;
    }

    _EARLY_KERNEL_END = .;

    /* Higher half kernel, linked at 3GB */
    _KERNEL_BASE = 0xC0000000;
    . += 0xC0000000;

    _KERNEL_START = .;

    .text ALIGN (4K) : AT(ADDR(.text) - _KERNEL_BASE) { *(.text*) }
    .rodata ALIGN (4K) : AT(ADDR(.rodata) - _KERNEL_BASE)
    {
        /* Kernel command line arguments */
        _ARGUMENTS_START = .;
        *(.arguments)
        _ARGUMENTS_END = .;
        /* Read-only data has to be last */
        *(.rodata*)
    }
    .data ALIGN (4K) : AT(ADDR(.data) - _KERNEL_BASE)
    {
        _CTORS_START = .;
        KEEP(*(.init_array));
        KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)));
        _CTORS_END = .;
        *(.data)
    }
    .bss ALIGN (4K) : AT(ADDR(.bss) - _KERNEL_BASE)
    {
        _BSS_START = .;
        *(.bss)
        *(COMMON)
        _BSS_END = .;
    }
    .page_tables ALIGN (4K) : AT(ADDR(.page_tables) - _KERNEL_BASE) { *(.page_tables) }

    _KERNEL_END = .;

    _EARLY_BSS_SIZE = _EARLY_BSS_END - _EARLY_BSS_START;
    _BSS_SIZE = _BSS_END - _BSS_START;
    _KERNEL_SIZE = _KERNEL_END - _KERNEL_START;

    /DISCARD/ : { *(.fini_array*) *(.comment) }
}

ASSERT(_KERNEL_SIZE < 0x700000, "Kernel exceeds the 7 MB limit. boot.s must be updated to work a larger kernel.");
