/**
 * This small function has to be written in assembly and is used
 * to flush the GDT and reload the new one onto the system.
 * Thanks to James Molloy for the documentation and to toaruOS
 * for the translation from NASM to AT&T.
 *
 * @brief This function is used to flush the GDT when we want to
 * enter into ring 3 (userspace) since we need to flush out all of
 * the kernel information and replace it with info safe for user
 * applications.
 */
.section .text
.align 4

.global gdt_flush   // Allows our code to call gdt_flush().

gdt_flush:
    /* Load GDT */
    mov 4(%esp), %eax
    lgdt (%eax)

    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %ss
    mov %ax, %gs

    ljmp $0x08, $.flush
.flush:
    ret

.global tss_flush   // Allows our code to call tss_flush().
tss_flush:
   mov $0x2B, %ax    // Load the index of our TSS structure - The index is
                     // 0x28, as it is the 5th selector and each is 8 bytes
                     // long, but we set the bottom two bits (making 0x2B)
                     // so that it has an RPL of 3, not zero.
   ltr %ax           // Load 0x2B into the task state register.
   ret
