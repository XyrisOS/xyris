; Defined in isr.c
extern isr_handler
extern irq_handler
align 4

; Common ISR code
isr_common_stub:
    ; 1. Save CPU state
    pushad              ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
    mov ax, ds          ; Lower 16-bits of eax = ds.
    push eax            ; save the data segment descriptor
    mov ax, 0x10        ; kernel data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    push esp            ; Push struct registers *r
    ; 2. Clear the direction flag (eflags) & call C handler
    cld                 ; C code following the sysV ABI requires DF to be clear on function entry
    call isr_handler

    ; 3. Restore state
    add esp, 4
    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    popad
    add esp, 8          ; Cleans up the pushed error code and pushed ISR number
    iret                ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP
    ; These irets need to be iretq's when in long mode

; Common IRQ code. Identical to ISR code except for the 'call'
; and the 'pop ebx'
irq_common_stub:
    pushad
    mov ax, ds
    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    push esp
    cld
    call irq_handler ; Different than the ISR code
    add esp, 4
    pop ebx          ; Different than the ISR code
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
    popad
    add esp, 8
    iret
    ; These irets need to be iretq's when in long mode

; Macro expansions for isr and irq

%macro m_isr_2 1

global isr%1
isr%1:
    push 0
    push %1
    jmp isr_common_stub

%endmacro

%macro m_isr_1 1

global isr%1
isr%1:
    push %1
    jmp isr_common_stub

%endmacro

%macro m_irq_2 2

global irq%1
irq%1:
    push %1
    push %2
    jmp irq_common_stub

%endmacro

; We don't get information about which interrupt was called
; when the handler is run, so we will need to have a different handler
; for every interrupt.
; Furthermore, some interrupts push an error code onto the stack but others
; don't, so we will push a dummy error code for those which don't, so that
; we have a consistent stack for all of them.

; All of the following Interrupt Service Routines are
; exception handlers. As such, they push the error code
; along with their exception (interrupt) number. If an
; exception has an error code, $0 will not be pushed as
; the CPU already does that for us.
;
m_isr_2 0 ; 0: Divide By Zero Exception
m_isr_2 1 ; 1: Debug Exception
m_isr_2 2 ; 2: Non Maskable Interrupt Exception
m_isr_2 3 ; 3: Int 3 Exception
m_isr_2 4 ; 4: INTO Exception
m_isr_2 5 ; 5: Out of Bounds Exception
m_isr_2 6 ; 6: Invalid Opcode Exception
m_isr_2 7 ; 7: Coprocessor Not Available Exception
m_isr_1 8 ; 8: Double Fault Exception (With Error Code!)
m_isr_2 9 ; 9: Coprocessor Segment Overrun Exception
m_isr_1 10 ; 10: Bad TSS Exception (With Error Code!)
m_isr_1 11 ; 11: Segment Not Present Exception (With Error Code!)
m_isr_1 12 ; 12: Stack Fault Exception (With Error Code!)
m_isr_1 13 ; 13: General Protection Fault Exception (With Error Code!)
m_isr_1 14 ; 14: Page Fault Exception (With Error Code!)
m_isr_2 15 ; 15: Reserved Exception
m_isr_2 16 ; 16: Floating Point Exception
m_isr_1 17 ; 17: Alignment Check Exception (With Error Code!)
m_isr_2 18 ; 18: Machine Check Exception
m_isr_2 19 ; 19: Reserved
m_isr_2 20 ; 20: Reserved
m_isr_2 21 ; 21: Reserved
m_isr_2 22 ; 22: Reserved
m_isr_2 23 ; 23: Reserved
m_isr_2 24 ; 24: Reserved
m_isr_2 25 ; 25: Reserved
m_isr_2 26 ; 26: Reserved
m_isr_2 27 ; 27: Reserved
m_isr_2 28 ; 28: Reserved
m_isr_2 29 ; 29: Reserved
m_isr_1 30 ; 30: Reserved (With Error Code!)
m_isr_2 31 ; 31: Reserved

; All of the following Interrupt Requests are
; stubs for hardware interrupts such as device interrupts.
; These are not exceptions. Each Interrupt Request pushes
; the IRQ value (starting at 0) along with their
; corresponding hardware interrupt value (starting at 32).
;
; Interrupt Request handlers
m_irq_2 0, 32 ; System timer (PIT)
m_irq_2 1, 33 ; PS2 Keyboard
m_irq_2 2, 34 ; PIC2
m_irq_2 3, 35 ; COM2
m_irq_2 4, 36 ; COM1
m_irq_2 5, 37 ; LPT2
m_irq_2 6, 38 ; Floppy Disk Drive
m_irq_2 7, 39 ; LPT1
m_irq_2 8, 40 ; CMOS Real Time Clock
m_irq_2 9, 41 ; Unused / Generic
m_irq_2 10, 42 ; Unused / Generic
m_irq_2 11, 43 ; Unused / Generic
m_irq_2 12, 44 ; PS2 Mouse
m_irq_2 13, 45 ; Numeric Coprocessor
m_irq_2 14, 46 ; IDE0 (HDD)
m_irq_2 15, 47 ; IDE1 (HDD)
