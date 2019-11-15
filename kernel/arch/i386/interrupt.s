 
// Defined in isr.c
.extern px_isr_handler
.extern px_irq_handler

// Common ISR code
isr_common_stub:
    // 1. Save CPU state
	pusha           // Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
	mov %ds, %ax    // Lower 16-bits of eax = ds.
	push %eax       // save the data segment descriptor
	mov %ax, 0x10  // kernel data segment descriptor
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	
    // 2. Call C handler
	call px_isr_handler
	
    // 3. Restore state
	pop %eax 
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	popa
	add $8, %esp    // Cleans up the pushed error code and pushed ISR number
	sti
	iret            // pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP

// Common IRQ code. Identical to ISR code except for the 'call' 
// and the 'pop ebx'
irq_common_stub:
    pusha 
    mov %ds, %ax
    push %eax
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    call px_irq_handler // Different than the ISR code
    pop %ebx  // Different than the ISR code
    mov %bx, %ds
    mov %bx, %es
    mov %bx, %fs
    mov %bx, %gs
    popa
    add $8, %esp
    sti
    iret 
	
// We don't get information about which interrupt was caller
// when the handler is run, so we will need to have a different handler
// for every interrupt.
// Furthermore, some interrupts push an error code onto the stack but others
// don't, so we will push a dummy error code for those which don't, so that
// we have a consistent stack for all of them.

// First make the ISRs global
.global isr0
.global isr1
.global isr2
.global isr3
.global isr4
.global isr5
.global isr6
.global isr7
.global isr8
.global isr9
.global isr10
.global isr11
.global isr12
.global isr13
.global isr14
.global isr15
.global isr16
.global isr17
.global isr18
.global isr19
.global isr20
.global isr21
.global isr22
.global isr23
.global isr24
.global isr25
.global isr26
.global isr27
.global isr28
.global isr29
.global isr30
.global isr31
// IRQs
.global irq0
.global irq1
.global irq2
.global irq3
.global irq4
.global irq5
.global irq6
.global irq7
.global irq8
.global irq9
.global irq10
.global irq11
.global irq12
.global irq13
.global irq14
.global irq15

// 0: Divide By Zero Exception
isr0:
    cli
    push 0
    push 0
    jmp isr_common_stub

// 1: Debug Exception
isr1:
    cli
    push 0
    push 1
    jmp isr_common_stub

// 2: Non Maskable Interrupt Exception
isr2:
    cli
    push 0
    push 2
    jmp isr_common_stub

// 3: Int 3 Exception
isr3:
    cli
    push 0
    push 3
    jmp isr_common_stub

// 4: INTO Exception
isr4:
    cli
    push 0
    push 4
    jmp isr_common_stub

// 5: Out of Bounds Exception
isr5:
    cli
    push 0
    push 5
    jmp isr_common_stub

// 6: Invalid Opcode Exception
isr6:
    cli
    push 0
    push 6
    jmp isr_common_stub

// 7: Coprocessor Not Available Exception
isr7:
    cli
    push 0
    push 7
    jmp isr_common_stub

// 8: Double Fault Exception (With Error Code!)
isr8:
    cli
    push 8
    jmp isr_common_stub

// 9: Coprocessor Segment Overrun Exception
isr9:
    cli
    push 0
    push 9
    jmp isr_common_stub

// 10: Bad TSS Exception (With Error Code!)
isr10:
    cli
    push 10
    jmp isr_common_stub

// 11: Segment Not Present Exception (With Error Code!)
isr11:
    cli
    push 11
    jmp isr_common_stub

// 12: Stack Fault Exception (With Error Code!)
isr12:
    cli
    push 12
    jmp isr_common_stub

// 13: General Protection Fault Exception (With Error Code!)
isr13:
    cli
    push 13
    jmp isr_common_stub

// 14: Page Fault Exception (With Error Code!)
isr14:
    cli
    push 14
    jmp isr_common_stub

// 15: Reserved Exception
isr15:
    cli
    push 0
    push 15
    jmp isr_common_stub

// 16: Floating Point Exception
isr16:
    cli
    push 0
    push 16
    jmp isr_common_stub

// 17: Alignment Check Exception
isr17:
    cli
    push 0
    push 17
    jmp isr_common_stub

// 18: Machine Check Exception
isr18:
    cli
    push 0
    push 18
    jmp isr_common_stub

// 19: Reserved
isr19:
    cli
    push 0
    push 19
    jmp isr_common_stub

// 20: Reserved
isr20:
    cli
    push 0
    push 20
    jmp isr_common_stub

// 21: Reserved
isr21:
    cli
    push 0
    push 21
    jmp isr_common_stub

// 22: Reserved
isr22:
    cli
    push 0
    push 22
    jmp isr_common_stub

// 23: Reserved
isr23:
    cli
    push 0
    push 23
    jmp isr_common_stub

// 24: Reserved
isr24:
    cli
    push 0
    push 24
    jmp isr_common_stub

// 25: Reserved
isr25:
    cli
    push 0
    push 25
    jmp isr_common_stub

// 26: Reserved
isr26:
    cli
    push 0
    push 26
    jmp isr_common_stub

// 27: Reserved
isr27:
    cli
    push 0
    push 27
    jmp isr_common_stub

// 28: Reserved
isr28:
    cli
    push 0
    push 28
    jmp isr_common_stub

// 29: Reserved
isr29:
    cli
    push 0
    push 29
    jmp isr_common_stub

// 30: Reserved
isr30:
    cli
    push 0
    push 30
    jmp isr_common_stub

// 31: Reserved
isr31:
    cli
    push 0
    push 31
    jmp isr_common_stub

// IRQ handlers
irq0:
	cli
	push 0
	push 32
	jmp irq_common_stub

irq1:
	cli
	push 1
	push 33
	jmp irq_common_stub

irq2:
	cli
	push 2
	push 34
	jmp irq_common_stub

irq3:
	cli
	push 3
	push 35
	jmp irq_common_stub

irq4:
	cli
	push 4
	push 36
	jmp irq_common_stub

irq5:
	cli
	push 5
	push 37
	jmp irq_common_stub

irq6:
	cli
	push 6
	push 38
	jmp irq_common_stub

irq7:
	cli
	push 7
	push 39
	jmp irq_common_stub

irq8:
	cli
	push 8
	push 40
	jmp irq_common_stub

irq9:
	cli
	push 9
	push 41
	jmp irq_common_stub

irq10:
	cli
	push 10
	push 42
	jmp irq_common_stub

irq11:
	cli
	push 11
	push 43
	jmp irq_common_stub

irq12:
	cli
	push 12
	push 44
	jmp irq_common_stub

irq13:
	cli
	push 13
	push 45
	jmp irq_common_stub

irq14:
	cli
	push 14
	push 46
	jmp irq_common_stub

irq15:
	cli
	push 15
	push 47
	jmp irq_common_stub
