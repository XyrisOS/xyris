; @brief This function is used to flush the GDT when we want to
; enter into ring 3 (userspace) since we need to flush out all of
; the kernel information and replace it with info safe for user
; applications. The following function flushes the TSS and can
; "flush" (invalidate) a page.
;
section .text
align 4

global gdt_flush
gdt_flush:
    ; Load GDT
    mov eax,[esp+4]
    lgdt [eax]

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov ss, ax
    mov gs, ax
    ; Do a long jump back and return
    jmp 0x08:.flush
.flush:
    ret

global tss_flush
tss_flush:
    mov ax, 0x2B    ; Load the index of our TSS structure - The index is
                    ; 0x28, as it is the 5th selector and each is 8 bytes
                    ; long, but we set the bottom two bits (making 0x2B)
                    ; so that it has an RPL of 3, not zero.
    ltr ax          ; Load 0x2B into the task state register.
    ret

global pageInvalidate
pageInvalidate:
    mov    eax, [esp+4]
    invlpg  [eax]
    ret
