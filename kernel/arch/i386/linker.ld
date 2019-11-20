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
   */
  . = 1M;

  /* Text section which contains our multiboot header and
   * all of our text data. We make it a 4 kilobyte block
   * that we align to a 4 kilobyte segment of memory.
   */
  .text BLOCK(4K) : ALIGN(4K)
  {
    *(.multiboot)
    *(.text*)
  }
  /* Read-Only Data that we give a 4 kilobyte block
   * that we align to a 4 kilobyte segment of memory
   * like in the text section above.
   */
  .rodata BLOCK(4K) : ALIGN(4K) {
    *(.rodata)
  }
  /*
   * Read-Write data that contains our global constructor
   * and all of our assembled kernel code.
   */
  .data BLOCK(4K) : ALIGN(4K)
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
  .bss BLOCK(4K) : ALIGN(4K)
  {
    *(COMMON)
    *(.bss)
  }

  /DISCARD/ : { *(.fini_array*) *(.comment) }
}