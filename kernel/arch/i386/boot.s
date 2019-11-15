.set MAGIC, 0x1badb002
.set FLAGS, (1<<0 | 1<<1)
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

.section .text
.extern px_kernel_main
.extern px_call_constructors
.global loader


loader:
    mov $kernel_stack, %esp
    call px_call_constructors
    push %eax
    push %ebx
    call px_kernel_main

# When the kernel is done we move down to stop, so we disable interrupts
# and then halt the CPU
_stop:
    cli
    hlt
    jmp _stop


.section .bss
.space 4*1024*1024; # 4 MiB
kernel_stack:
