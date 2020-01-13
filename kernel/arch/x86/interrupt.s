# Defined in isr.c
.extern px_isr_handler
.extern px_irq_handler

# Common ISR code
isr_common_stub:
    # 1. Save CPU state
    pusha               # Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

    movw %ds, %ax       # Lower 16-bits of eax = ds.
    pushl %eax          # save the data segment descriptor
    pushl %esp          # Push registers_t *r

    movw $0x10, %ax     # kernel data segment descriptor
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    # 2. Clear the directory flag (eflags) & call C handler
    # cld                 # C code following the sysV ABI requires DF to be clear on function entry
    call px_isr_handler

    # 3. Restore state
    addl $4, %esp
    popl %eax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    popa
    addl $8, %esp       # Cleans up the pushed error code and pushed ISR number
    sti
    iret                # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP

# Common IRQ code. Identical to ISR code except for the 'call' 
# and the 'pop ebx'
irq_common_stub:
    pusha
    movw %ds, %ax
    pushl %eax
    pushl %esp

    movw $0x10, %ax

    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    #cld                 # C code following the sysV ABI requires DF to be clear on function entry
    call px_irq_handler # Different than the ISR code
    addl $4, %esp
    popl %ebx           # Different than the ISR code
    movw %bx, %ds
    movw %bx, %es
    movw %bx, %fs
    movw %bx, %gs

    popa
    addl $8, %esp
    sti
    iret

# We don't get information about which interrupt was called
# when the handler is run, so we will need to have a different handler
# for every interrupt.
# Furthermore, some interrupts push an error code onto the stack but others
# don't, so we will push a dummy error code for those which don't, so that
# we have a consistent stack for all of them.
#
# All of the following Interrupt Service Routines are
# exception handlers. As such, they push the error code
# along with their exception (interrupt) number. If an
# exception has an error code, $0 will not be pushed as
# the CPU already does that for us.
#
# Note: Thanks to grant-h for his code, which we used as a reference to help
# bug fix some of this. His code is much nicer since NASM supports macros,
# which almost makes me want to switch.
#
# 0: Divide By Zero Exception
.global isr0
isr0:
    cli
    push $0
    push $0
    jmp isr_common_stub

# 1: Debug Exception
.global isr1
isr1:
    cli
    push $0
    push $1
    jmp isr_common_stub

# 2: Non Maskable Interrupt Exception
.global isr2
isr2:
    cli
    push $0
    push $2
    jmp isr_common_stub

# 3: Int 3 Exception
.global isr3
isr3:
    cli
    push $0
    push $3
    jmp isr_common_stub

# 4: INTO Exception
.global isr4
isr4:
    cli
    push $0
    push $4
    jmp isr_common_stub

# 5: Out of Bounds Exception
.global isr5
isr5:
    cli
    push $0
    push $5
    jmp isr_common_stub

# 6: Invalid Opcode Exception
.global isr6
isr6:
    cli
    push $0
    push $6
    jmp isr_common_stub

# 7: Coprocessor Not Available Exception
.global isr7
isr7:
    cli
    push $0
    push $7
    jmp isr_common_stub

# 8: Double Fault Exception (With Error Code!)
.global isr8
isr8:
    cli
    push $8
    jmp isr_common_stub

# 9: Coprocessor Segment Overrun Exception
.global isr9
isr9:
    cli
    push $0
    push $9
    jmp isr_common_stub

# 10: Bad TSS Exception (With Error Code!)
.global isr10
isr10:
    cli
    push $10
    jmp isr_common_stub

# 11: Segment Not Present Exception (With Error Code!)
.global isr11
isr11:
    cli
    push $11
    jmp isr_common_stub

# 12: Stack Fault Exception (With Error Code!)
.global isr12
isr12:
    cli
    push $12
    jmp isr_common_stub

# 13: General Protection Fault Exception (With Error Code!)
.global isr13
isr13:
    cli
    push $13
    jmp isr_common_stub

# 14: Page Fault Exception (With Error Code!)
.global isr14
isr14:
    cli
    push $14
    jmp isr_common_stub

# 15: Reserved Exception
.global isr15
isr15:
    cli
    push $0
    push $15
    jmp isr_common_stub

# 16: Floating Point Exception
.global isr16
isr16:
    cli
    push $0
    push $16
    jmp isr_common_stub

# 17: Alignment Check Exception (With Error Code!)
.global isr17
isr17:
    cli
    push $17
    jmp isr_common_stub

# 18: Machine Check Exception
.global isr18
isr18:
    cli
    push $0
    push $18
    jmp isr_common_stub

# 19: Reserved
.global isr19
isr19:
    cli
    push $0
    push $19
    jmp isr_common_stub

# 20: Reserved
.global isr20
isr20:
    cli
    push $0
    push $20
    jmp isr_common_stub

# 21: Reserved
.global isr21
isr21:
    cli
    push $0
    push $21
    jmp isr_common_stub

# 22: Reserved
.global isr22
isr22:
    cli
    push $0
    push $22
    jmp isr_common_stub

# 23: Reserved
.global isr23
isr23:
    cli
    push $0
    push $23
    jmp isr_common_stub

# 24: Reserved
.global isr24
isr24:
    cli
    push $0
    push $24
    jmp isr_common_stub

# 25: Reserved
.global isr25
isr25:
    cli
    push $0
    push $25
    jmp isr_common_stub

# 26: Reserved
.global isr26
isr26:
    cli
    push $0
    push $26
    jmp isr_common_stub

# 27: Reserved
.global isr27
isr27:
    cli
    push $0
    push $27
    jmp isr_common_stub

# 28: Reserved
.global isr28
isr28:
    cli
    push $0
    push $28
    jmp isr_common_stub

# 29: Reserved
.global isr29
isr29:
    cli
    push $0
    push $29
    jmp isr_common_stub

# 30: Reserved (With Error Code!)
.global isr30
isr30:
    cli
    push $30
    jmp isr_common_stub

# 31: Reserved
.global isr31
isr31:
    cli
    push $0
    push $31
    jmp isr_common_stub

# All of the following Interrupt Requests are
# stubs for hardware interrupts such as device interrupts.
# These are not exceptions. Each Interrupt Request pushes
# the IRQ value (starting at 0) along with their
# corresponding hardware interrupt value (starting at 32).
#
# IRQ handlers
# System timer (PIT)
.global irq0
irq0:
        cli
        push $0
        push $32
        jmp irq_common_stub
# PS2 Keyboard
.global irq1
irq1:
        cli
        push $0
        push $33
        jmp irq_common_stub
# PIC2
.global irq2
irq2:
        cli
        push $0
        push $34
        jmp irq_common_stub
# COM2
.global irq3
irq3:
        cli
        push $0
        push $35
        jmp irq_common_stub
# COM1
.global irq4
irq4:
        cli
        push $0
        push $36
        jmp irq_common_stub
# LPT2
.global irq5
irq5:
        cli
        push $0
        push $37
        jmp irq_common_stub
# Floppy Disk Drive
.global irq6
irq6:
        cli
        push $0
        push $38
        jmp irq_common_stub
# LPT1
.global irq7
irq7:
        cli
        push $0
        push $39
        jmp irq_common_stub
# CMOS Real Time Clock
.global irq8
irq8:
        cli
        push $0
        push $40
        jmp irq_common_stub
# Unused / Generic
.global irq9
irq9:
        cli
        push $0
        push $41
        jmp irq_common_stub
# Unused / Generic
.global irq10
irq10:
        cli
        push $0
        push $42
        jmp irq_common_stub
# Unused / Generic
.global irq11
irq11:
        cli
        push $0
        push $43
        jmp irq_common_stub
# PS2 Mouse
.global irq12
irq12:
        cli
        push $0
        push $44
        jmp irq_common_stub
# Numeric Coprocessor
.global irq13
irq13:
        cli
        push $0
        push $45
        jmp irq_common_stub
# IDE0 (HDD)
.global irq14
irq14:
        cli
        push $0
        push $46
        jmp irq_common_stub
# IDE1 (HDD)
.global irq15
irq15:
        cli
        push $0
        push $47
        jmp irq_common_stub
