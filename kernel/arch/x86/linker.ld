/* Tell our linker that our entry point is the boot_loader function
 * which we defined in the boot.s file.
 */
ENTRY(boot_loader)
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)

SECTIONS
{
  /* This line signifies where we want to start putting our
   * data sections in memory. This tells us that we want to
   * start at the 1 megabyte mark, which makes it a bit
   * easier for us to grab data from later.
   *
   * The kernel will live at 3GB + 1MB in the virtual address space,
	 * which will be mapped to 1MB in the physical address space.
	 * Note that we page-align the sections.
   * 0xC0000000 = 3GB mark
   * 0x00100000 = 1MB mark
   *
   * Please read this excellent blog post explaining why a higher
   * kernel is important for userspace and virtual memory:
   * https://medium.com/@connorstack/how-does-a-higher-half-kernel-work-107194e46a64
   */
	. = 0xC0100000;
  /* Add a symbol that indicates the start address of the kernel. */
  _kernel_start = .;

  /* Text section which contains our multiboot header and
   * all of our text data. We make it a 4 kilobyte block
   * that we align to a 4 kilobyte segment of memory.
   */
  .text ALIGN(4K) : AT (ADDR (.text) - 0xC0000000)
  {
    *(.multiboot)
    *(.text*)
  }
  /* Read-Only Data that we give a 4 kilobyte block
   * that we align to a 4 kilobyte segment of memory
   * like in the text section above.
   */
  .rodata ALIGN (4K) : AT (ADDR (.rodata) - 0xC0000000)
  {
    *(.rodata)
  }
  /*
   * Read-Write data that contains our global constructor
   * and all of our assembled kernel code.
   */
  .data ALIGN (4K) : AT (ADDR (.data) - 0xC0000000)
  {
    /* Global constructors data. This was leftover from the WYOOS tutorial 
     * and if I'm being honest, I don't understand it at all because no one
     * on the OSDev Wiki does it like this. 
     */
    start_ctors = .;
    KEEP(*( .init_array ));
    KEEP(*(SORT_BY_INIT_PRIORITY( .init_array.* )));
    end_ctors = .;

    *(.data)
  }
  /*
   * Uninitialized Read-Write data and our Stack
   */
  .bss ALIGN (4K) : AT (ADDR (.bss) - 0xC0000000)
  {
    *(COMMON)
    *(.bss)
    *(.bootstrap_stack)
  }
  /* Add a symbol that indicates the end address of the kernel. */
	_kernel_end = .;

  /DISCARD/ : { *(.fini_array*) *(.comment) }
}