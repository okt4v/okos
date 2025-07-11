section .multiboot
align 4
multiboot_header:
    dd 0x1BADB002              ; Magic number
    dd 0x00000003              ; Flags
    dd -(0x1BADB002 + 0x03)    ; Checksum

section .text
bits 32
global _start
extern kernel_main

_start:
    cli
    mov esp, 0x80000           ; Set up stack

    ; Verify Multiboot
    cmp eax, 0x2BADB002        ; GRUB's magic value
    jne no_multiboot

    ; Set up page tables (simplified)
    mov eax, pml4_table
    mov cr3, eax

    ; Enable PAE
    mov eax, cr4
    or eax, (1 << 5)
    mov cr4, eax

    ; Enable long mode
    mov ecx, 0xC0000080        ; EFER MSR
    rdmsr
    or eax, (1 << 8)           ; LME bit
    wrmsr

    ; Enable paging
    mov eax, cr0
    or eax, (1 << 31)          ; PG bit
    mov cr0, eax

    ; Load GDT and jump to 64-bit
    lgdt [gdt64_pointer]
    jmp gdt64_code:long_mode

no_multiboot:
    mov al, 'M'
    jmp no_multiboot

; 64-bit GDT
align 8
gdt64:
    dq 0
gdt64_code: equ $ - gdt64
    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53) ; 64-bit code
gdt64_pointer:
    dw $ - gdt64 - 1
    dq gdt64

; idt setup for keaboard interupts
align 8 
idt:
  times 256 dq 0 
idt_pointer:
  dw 256 * 16 - 1 
  dq idt

; remap pic and enable keyboard interupts
setup_interupts:
; Remap PIC
    mov al, 0x11
    out 0x20, al    ; ICW1 to master
    out 0xA0, al    ; ICW1 to slave
    mov al, 0x20
    out 0x21, al    ; Master offset = 0x20
    mov al, 0x28
    out 0xA1, al    ; Slave offset = 0x28
    mov al, 0x04
    out 0x21, al    ; Tell master about slave
    mov al, 0x02
    out 0xA1, al    ; Tell slave its cascade
    mov al, 0x01
    out 0x21, al    ; 8086 mode
    out 0xA1, al

; Page tables (simplified)
align 4096
pml4_table:
    dq pdpt_table + 0x3        ; Present + RW
    times 511 dq 0
pdpt_table:
    dq pd_table + 0x3
    times 511 dq 0
pd_table:
    dq 0x83                    ; 2MB page, Present + RW + PS
    times 511 dq 0

; 64-bit mode entry
bits 64
long_mode:
    mov ax, 0 
    mov ss, ax 
    mov ds, ax 
    mov es, ax 
    mov fs, ax 
    mov gs, ax 
    mov gs, ax 
    mov rsp, 0x80000           ; Update stack pointer
    call kernel_main           ; Jump to kernel
    hlt
