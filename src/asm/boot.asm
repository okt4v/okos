bits 32

section .multiboot
align 8
multiboot_header_start:
    dd 0xe85250d6
    dd 0
    dd multiboot_header_end - multiboot_header_start
    dd -(0xe85250d6 + 0 + (multiboot_header_end - multiboot_header_start))
    
    align 8
    dw 0
    dw 0
    dd 8
multiboot_header_end:

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:

align 4096
p4_table:
    resb 4096
p3_table:
    resb 4096
p2_table:
    resb 4096

section .text
global _start
extern kernel_main

_start:
    mov esp, stack_top
    mov edi, ebx
    mov esi, eax
    
    call setup_page_tables
    call enable_paging
    
    lgdt [gdt64.pointer]
    
    jmp 0x08:long_mode_start

setup_page_tables:
    mov eax, p3_table
    or eax, 0b11
    mov [p4_table], eax
    
    mov eax, p2_table
    or eax, 0b11
    mov [p3_table], eax
    
    mov ecx, 0
.map_p2_table:
    mov eax, 0x200000
    mul ecx
    or eax, 0b10000011
    mov [p2_table + ecx * 8], eax
    inc ecx
    cmp ecx, 512
    jne .map_p2_table
    
    ret

enable_paging:
    mov eax, p4_table
    mov cr3, eax
    
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax
    
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr
    
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax
    
    ret

bits 64
long_mode_start:
    ; Set up stack first
    mov rsp, 0x200000
    and rsp, -16

    ; Load data segment selector (0x10) into segment registers
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Set SS by pushing it and using far return trick
    ; Push SS (0x10), push RSP, push RFLAGS, push CS (0x08), push return address
    push 0x10          ; SS
    push rsp           ; RSP
    pushfq             ; RFLAGS
    push 0x08          ; CS
    lea rax, [rel .reload_segments]
    push rax
    iretq

.reload_segments:
    ; Now SS is properly set to 0x10
    
    ; Call kernel (rdi and rsi already set from edi/esi)
    call kernel_main
    
    ; Hang if kernel returns
    cli
.hang:
    hlt
    jmp .hang

section .rodata
align 16
gdt64:
    dq 0                                           ; Null descriptor
.code: equ 0x08
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53)      ; Code: exec, present, 64-bit
.data: equ 0x10
    dq (1<<41) | (1<<44) | (1<<47)                ; Data: writable, present
.pointer:
    dw 23                                          ; Limit (3 entries * 8 - 1)
    dq gdt64                                       ; Base
