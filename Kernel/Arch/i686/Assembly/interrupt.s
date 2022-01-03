; Defined in isr.c
extern exceptionHandler
extern interruptHandler
align 4

; Common ISR code
exception_stub:
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
    call exceptionHandler

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
interrupt_stub:
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
    call interruptHandler ; Different than the ISR code
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

%macro m_exception_err 1

global exception%1
exception%1:
    push 0
    push %1
    jmp exception_stub

%endmacro

%macro m_exception_no_err 1

global exception%1
exception%1:
    push %1
    jmp exception_stub

%endmacro

%macro m_interrupt 2

global interrupt%1
interrupt%1:
    push %1
    push %2
    jmp interrupt_stub

%endmacro

; We don't get information about which interrupt was called when the handler
; is run, so we will need to have a different handler for every interrupt.
; Furthermore, some interrupts push an error code onto the stack but others
; don't, so we will push a dummy error code for those which don't, so that
; we have a consistent stack for all of them.

; All of the following Interrupt Service Routines are exception handlers.
; As such, they push the error code along with their exception (interrupt)
; number. If an exception has an error code, $0 will not be pushed as the CPU
; already does that for us.
;
m_exception_err 0       ; 0: Divide By Zero Exception
m_exception_err 1       ; 1: Debug Exception
m_exception_err 2       ; 2: Non Maskable Interrupt Exception
m_exception_err 3       ; 3: Int 3 Exception
m_exception_err 4       ; 4: INTO Exception
m_exception_err 5       ; 5: Out of Bounds Exception
m_exception_err 6       ; 6: Invalid Opcode Exception
m_exception_err 7       ; 7: Coprocessor Not Available Exception
m_exception_no_err 8    ; 8: Double Fault Exception (With Error Code!)
m_exception_err 9       ; 9: Coprocessor Segment Overrun Exception
m_exception_no_err 10   ; 10: Bad TSS Exception (With Error Code!)
m_exception_no_err 11   ; 11: Segment Not Present Exception (With Error Code!)
m_exception_no_err 12   ; 12: Stack Fault Exception (With Error Code!)
m_exception_no_err 13   ; 13: General Protection Fault Exception (With Error Code!)
m_exception_no_err 14   ; 14: Page Fault Exception (With Error Code!)
m_exception_err 15      ; 15: Reserved Exception
m_exception_err 16      ; 16: Floating Point Exception
m_exception_no_err 17   ; 17: Alignment Check Exception (With Error Code!)
m_exception_err 18      ; 18: Machine Check Exception
m_exception_err 19      ; 19: Reserved
m_exception_err 20      ; 20: Reserved
m_exception_err 21      ; 21: Reserved
m_exception_err 22      ; 22: Reserved
m_exception_err 23      ; 23: Reserved
m_exception_err 24      ; 24: Reserved
m_exception_err 25      ; 25: Reserved
m_exception_err 26      ; 26: Reserved
m_exception_err 27      ; 27: Reserved
m_exception_err 28      ; 28: Reserved
m_exception_err 29      ; 29: Reserved
m_exception_no_err 30   ; 30: Reserved (With Error Code!)
m_exception_err 31      ; 31: Reserved

; All of the following Interrupt Requests are stubs for hardware interrupts such
; as device interrupts. These are not exceptions. Each Interrupt Request pushes
; the IRQ value (starting at 0) along with their corresponding hardware interrupt
; value (starting at 32).
;
; Interrupt Request handlers
m_interrupt 0, 32       ; System timer (PIT)
m_interrupt 1, 33       ; PS2 Keyboard
m_interrupt 2, 34       ; PIC2
m_interrupt 3, 35       ; COM2
m_interrupt 4, 36       ; COM1
m_interrupt 5, 37       ; LPT2
m_interrupt 6, 38       ; Floppy Disk Drive
m_interrupt 7, 39       ; LPT1
m_interrupt 8, 40       ; CMOS Real Time Clock
m_interrupt 9, 41       ; Unused / Generic
m_interrupt 10, 42      ; Unused / Generic
m_interrupt 11, 43      ; Unused / Generic
m_interrupt 12, 44      ; PS2 Mouse
m_interrupt 13, 45      ; Numeric Coprocessor
m_interrupt 14, 46      ; IDE0 (HDD)
m_interrupt 15, 47      ; IDE1 (HDD)
