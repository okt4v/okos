bits 32

section .multiboot
align 8
multiboot_header_start:
  dd 0xe85250d6
  dd 0 
  dd multiboot_header_end - multiboot_header_start 
  dd -(0xe85250d6 + 0 + (multiboot_header_end - multiboot_header_start))

  align 8 
framebuffer_tag_start:
  dw 5 
  dw 1 
  dd framebuffer_tag_end - framebuffer_tag_start
  dd 1024
  dd 768 
  dd 32 
framebuffer_tag_end:

  align 8
  dw 0 
  dw 0 
  dd 8 
multiboot_header_end:

section .bss 
align 16
stack_bottom:
  resb 16384    ;16 KiB stack 
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
  call setup_long_mode

  lgdt [gdt64.pointer]

  jmp gdt64.code:long_mode_start

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

setup_long_mode:
  ret 

bits 64 
long_mode_start:
  mov ax, gdt64.data 
  mov ss, ax 
  mov ds, ax 
  mov es, ax 
  mov fs, ax 
  mov gs, ax 

  call kernel_main

  cli
.hang:
  hlt 
  jmp .hang

section .rodata 
gdt64:
  dq 0 
.code: equ $ - gdt64
  dq (1<<43) | (1<<44) | (1<<47) | (1<<53)
.data: equ $ - gdt64
  dq (1<<44) | (1<<47)
.pointer:
  dw $ - gdt64 - 1 
  dq gdt64 

