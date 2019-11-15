/**
 * This small function has to be written in assembly and is used
 * to flush the GDT and reload the new one onto the system.
 * Thanks to James Molloy for the documentation and to toaruOS
 * for the translation from NASM to AT&T.
 */
.section .text
.align 4

.global gdt_flush
.type gdt_flush, @function

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