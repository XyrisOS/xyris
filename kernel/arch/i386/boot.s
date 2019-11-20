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

# Text section of our executable. See linker.ld
.section .text
.extern px_kernel_main
.extern px_call_constructors
.global boot_loader
.type boot_loader @function
boot_loader:
    mov $kernel_stack_top, %esp
    # Call the global constructor we defined in main.cpp
    call px_call_constructors
    push %eax
    push %ebx
    call px_kernel_main
    # By this point we should be into the wild world of C++
    # So, this should never be called unless the kernel returns
    jmp _stop

# When the kernel is done we move down to stop, so we disable interrupts
# and then halt the CPU
_stop:
    cli
    hlt
    jmp _stop

# BSS section which reserves 16 Kb space for our kernel thread stack.
# See linker.ld
.section .bss
.align 16
kernel_stack_bottom:
.space 16384; # 16 Kb
kernel_stack_top: