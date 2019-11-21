.set MULTIBOOT_MAGIC,       0x1badb002
.set MULTIBOOT_PAGE_ALIGN,  1<<0
.set MULTIBOOT_MEM_INFO,    1<<1
.set MULTIBOOT_FLAGS,       (MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEM_INFO)
.set MULTIBOOT_CHECKSUM,    -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

# Multiboot header section of our executable. See linker.ld
.section .multiboot
.long MULTIBOOT_MAGIC
.long MULTIBOOT_FLAGS
.long MULTIBOOT_CHECKSUM

# Bootstrap Stack Section which reserves 16 Kb space for our kernel thread stack.
# See linker.ld
.section .bootstrap_stack, "aw", @nobits # former .bss section
.align 16
kernel_stack_bottom:
.space 16384; # 16 Kb stack size
kernel_stack_top:

# Preallocate pages used for paging. Don't hard-code addresses and assume they
# are available, as the bootloader might have loaded its multiboot structures or
# modules there. This lets the bootloader know it must avoid the addresses.
.section .bss, "aw", @nobits
	.align 4096
boot_page_directory:
	.skip 4096
boot_page_table1:
	.skip 4096
# Further page tables may be required if the kernel grows beyond 3 MiB.

# Text section of our executable. See linker.ld
# This is the kernel entry point
.section .text
# external reference to our global constructors and kernel main functions
# which are defined in our main.cpp file. This allows assembly to call
# function in C++ by telling the compiler they exist "somewhere"
.extern px_kernel_main
.extern px_call_constructors
.global boot_loader
.type boot_loader @function
boot_loader:
    # Physical address of boot_page_table1.
	movl $(boot_page_table1 - 0xC0000000), %edi
	# First address to map is address 0.
	# TODO: Start at the first kernel page instead. Alternatively map the first
	#       1 MiB as it can be generally useful, and there's no need to
	#       specially map the VGA buffer.
	movl $0, %esi
	# Map 1023 pages. The 1024th will be the VGA text buffer.
	movl $1023, %ecx
1:
    # Only map the kernel.
	cmpl $(_kernel_start - 0xC0000000), %esi
	jl 2f
	cmpl $(_kernel_end - 0xC0000000), %esi
	jge 3f
	# Map physical address as "present, writable". Note that this maps
	# .text and .rodata as writable. Mind security and map them as non-writable.
	movl %esi, %edx
	orl $0x003, %edx
	movl %edx, (%edi)

2:
    # Size of page is 4096 bytes.
	addl $4096, %esi
	# Size of entries in boot_page_table1 is 4 bytes.
	addl $4, %edi
	# Loop to the next entry if we haven't finished.
	loop 1b

3:
	# Map VGA video memory to 0xC03FF000 as "present, writable".
	movl $(0x000B8000 | 0x003), boot_page_table1 - 0xC0000000 + 1023 * 4

	# The page table is used at both page directory entry 0 (virtually from 0x0
	# to 0x3FFFFF) (thus identity mapping the kernel) and page directory entry
	# 768 (virtually from 0xC0000000 to 0xC03FFFFF) (thus mapping it in the
	# higher half). The kernel is identity mapped because enabling paging does
	# not change the next instruction, which continues to be physical. The CPU
	# would instead page fault if there was no identity mapping.

	# Map the page table to both virtual addresses 0x00000000 and 0xC0000000.
	movl $(boot_page_table1 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 0
	movl $(boot_page_table1 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 768 * 4

	# Set cr3 to the address of the boot_page_directory.
	movl $(boot_page_directory - 0xC0000000), %ecx
	movl %ecx, %cr3

	# Enable paging and the write-protect bit.
	movl %cr0, %ecx
	orl $0x80010000, %ecx
	movl %ecx, %cr0

	# Jump to higher half with an absolute jump. 
	lea 4f, %ecx
	jmp *%ecx

4:
	# At this point, paging is fully set up and enabled.

	# Unmap the identity mapping as it is now unnecessary. 
	movl $0, boot_page_directory + 0

	# Reload crc3 to force a TLB flush so the changes to take effect.
	movl %cr3, %ecx
	movl %ecx, %cr3

	# Set up the stack.
	mov $kernel_stack_top, %esp

	# Enter the high-level kernel.
    call px_kernel_main
    # By this point we should be into the wild world of C++
    # So, this should never be called unless the kernel returns
    cli
    jmp _stop

# When the kernel is done we move down to stop, so we disable interrupts
# and then halt the CPU
_stop:
    hlt
    jmp _stop
