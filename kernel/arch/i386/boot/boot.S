; make the _start function available to the linker
global _start

; global constructor and destructor calls
extern _init
extern _fini

; external reference to our global constructors and kernel main functions
; which are defined in our main.cpp file. This allows assembly to call
; function in C++ by telling the compiler they exist "somewhere"
extern kernel_main
; minimal panic function that works in most situations
extern early_panic

extern _KERNEL_BASE
extern _KERNEL_START
extern _KERNEL_END
extern _BSS_START
extern _BSS_SIZE

extern _EARLY_KERNEL_START
extern _EARLY_KERNEL_END
extern _EARLY_BSS_START
extern _EARLY_BSS_SIZE

LOWMEM_END equ _EARLY_KERNEL_END        ; lowmem ends at the 1st MB
PAGE_SIZE equ 4096
PAGE_SHIFT equ 12                       ; 2^12 = 4096 = PAGE_SIZE
PAGE_PERM equ 3                         ; default page permissions: present, read/write
STACK_SIZE equ 4*PAGE_SIZE              ; initial kernel stack space size of 16k

; Text section of our executable. See linker.ld
; This is the kernel entry point
section .early_text alloc exec progbits
_start:
    ; save our multiboot information from grub before messing with registers
    ; these can't be saved on the stack as we are about to zero it
    mov dword [multiboot_magic], eax
    mov dword [multiboot_info], ebx

    ; zero the early BSS to start things off well
    mov ecx, _EARLY_BSS_SIZE
    xor al, al
    mov edi, _EARLY_BSS_START
    rep
    stosb

    ; identity map from 0x00000000 -> LOWMEM_END
    ; WARNING: code assumes that the kernel won't be greater than 3MB
    mov eax, lowmem_pt
    mov [page_directory], eax
    or dword [page_directory], PAGE_PERM

    xor eax, eax

_start.lowmem:
    mov ecx, eax
    shr ecx, PAGE_SHIFT
    and ecx, 0x3ff
    mov [ecx * 4 + lowmem_pt], eax
    or dword [ecx * 4 + lowmem_pt], PAGE_PERM

    add eax, PAGE_SIZE
    cmp eax, LOWMEM_END
    jl _start.lowmem

    ; create virtual mappings for the kernel in the higher-half
    mov edx, _KERNEL_START
    shr edx, 22       ; find which directory entry we need to use

        ; the first page table
    mov eax, kernel_pt
    mov [edx * 4 + page_directory], eax
    or dword [edx * 4 + page_directory], PAGE_PERM

        ; the second page table
        inc edx
        mov eax, pages_pt
        mov [edx * 4 + page_directory], eax
        or dword [edx * 4 + page_directory], PAGE_PERM

    mov eax, _KERNEL_START ; the kernel's current virtual start

_start.higher:
        ; compute the page table offset
        ; this only works because the two page tables are adjacent
    mov edx, eax
        shr edx, 22
        sub edx, 0x300
        imul edx, PAGE_SIZE

        mov ecx, eax
    shr ecx, PAGE_SHIFT
    and ecx, 0x3ff ; generate kernel PTE index

    mov ebx, eax
    sub ebx, _KERNEL_BASE ; convert virt->physical
    mov [edx + ecx * 4 + kernel_pt], ebx
    or dword [edx + ecx * 4 + kernel_pt], PAGE_PERM

    add eax, PAGE_SIZE  ; move on to the next page
    cmp eax, _KERNEL_END ; are we done mapping in the kernel?
    jl _start.higher

    mov eax, page_directory
    mov cr3, eax ; load CR3 with our page directory

    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax ; enable paging! make sure the next instruction fetch doesnt page fault

    ; zero the kernel BSS
    mov ecx, _BSS_SIZE
    xor al, al
    mov edi, _BSS_START
    rep
    stosb

    ; adjust the stack in to the virtual area
    ; setup and adjust the stack
    mov esp, stack + STACK_SIZE

    ; check if SSE is available
    mov eax, 0x1
    cpuid
    test edx, 1<<25
    jnz _start.has_sse

    ; panic because no SSE :'(
    push no_sse_msg
    call early_panic

    ; infinite loop if panic returns
    jmp _start.catchfire

_start.has_sse:
    ; enable SSE
    mov eax, cr0
    and ax, 0xFFFB  ; clear coprocessor emulation CR0.EM
    or ax, 0x2      ; set coprocessor monitoring  CR0.MP
    mov cr0, eax
    mov eax, cr4
    or ax, 3<<9 ; set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
    mov cr4, eax

    ; Set NULL stack frame for trace
    xor ebp, ebp

    ; Call global constructors
    call _init

    ; push kernel main parameters
    push dword [multiboot_magic]         ; Multiboot magic number
    push dword [multiboot_info]          ; Multiboot info structure

    ; Enter the high-level kernel.
    call kernel_main

    ; Call global destructors
    call _fini

    ; By this point we should be into the wild world of C++
    ; So, this should never be called unless the kernel returns
    _start.catchfire:
    hlt
    jmp _start.catchfire

section .early_bss alloc write nobits
align 4096
page_directory: resd 1024    ; should also be page aligned (hopefully)
                             ; reserve 1024 DWORDs for our page table pointers
lowmem_pt: resd 1024         ; lowmem identity mappings
kernel_pt: resd 1024         ; our kernel page table mappings
pages_pt: resd 1024          ; a page table that maps pages that contain page tables

section .early_data alloc write
align 4
multiboot_magic: dd 0
multiboot_info: dd 0
no_sse_msg: db "Error: No SSE support available!", 0

section .bss alloc write nobits
align 4
stack:
    resb STACK_SIZE

; vim: ft=nasm :
