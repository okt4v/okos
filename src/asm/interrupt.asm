section .text

extern isr_handler

%macro ISR_NOERRCODE 1
global isr%1
isr%1:
  push 0
  push %1
  jmp isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
global isr%1
isr%1:
  push %1
  jmp isr_common_stub
%endmacro

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_ERRCODE   13
ISR_ERRCODE   14

isr_common_stub:

  push rax
  push rbx
  push rcx
  push rdx
  push rsi
  push rdi
  push rbp
  push r8
  push r9
  push r10
  push r11
  push r12
  push r13
  push r14
  push r15

  mov rdi, [rsp + 120]
  mov rsi, [rsp + 128]
  call isr_handler

  pop r15
  pop r14
  pop r13
  pop r12
  pop r11
  pop r10
  pop r9
  pop r8
  pop rbp
  pop rdi
  pop rsi
  pop rdx
  pop rcx
  pop rbx
  pop rax

  add rsp, 16
  iretq

global idt_load
idt_load:
  lidt [rdi]
  ret
