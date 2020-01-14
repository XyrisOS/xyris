/* Tell our linker that our entry point is the boot_loader function
 * which we defined in the boot.s file.
 */
ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)

SECTIONS
{
  /* Early kernel section, linked at 1MB */
  . = 0x00100000;
  _EARLY_KERNEL_START = .;

  .__mbHeader : { *(.__mbHeader) }
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
  . += 0xC0000000;

  _KERNEL_START = .;

  .text ALIGN (4K) : AT(ADDR(.text) - 0xC0000000) { *(.text) }
  .rodata ALIGN (4K) : AT(ADDR(.rodata) - 0xC0000000) { *(.rodata*) }
  .data ALIGN (4K) : AT(ADDR(.data) - 0xC0000000)
  {
    _CTORS_START = .;
    KEEP(*(.init_array));
    KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)));
    _CTORS_END = .;
    *(.data)
  }
  .bss ALIGN (4K) : AT(ADDR(.bss) - 0xC0000000)
  {
    _BSS_START = .;
    *(.bss)
    *(COMMON)
    _BSS_END = .;
  }
  .early_kmalloc ALIGN (4K) : AT(ADDR(.early_kmalloc) - 0xC0000000)
  {
    _EARLY_KMALLOC_START = .;
    . += 4K; /* 1MB of heap space */
    _EARLY_KMALLOC_END = .;
  }

  _KERNEL_END = .;

  _EARLY_BSS_SIZE = _EARLY_BSS_END - _EARLY_BSS_START;
  _BSS_SIZE = _BSS_END - _BSS_START;
  _KERNEL_SIZE = _KERNEL_END - _KERNEL_START;

  /DISCARD/ : { *(.fini_array*) *(.comment) }
}

ASSERT(_KERNEL_SIZE < 0x300000, "Kernel exceeds the 3 MB limit. loader.s must be updated to work a larger kernel.");
