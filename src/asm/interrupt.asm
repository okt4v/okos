section .text

extern isr_handler
extern irq_handler

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

%macro IRQ 2
global isr%1
isr%1:
  push 0
  push %2
  jmp irq_common_stub
%endmacro

ISR_NOERRCODE 0   ; Division By Zero
ISR_NOERRCODE 1   ; Debug
ISR_NOERRCODE 2   ; Non Maskable Interrupt
ISR_NOERRCODE 3   ; Breakpoint
ISR_NOERRCODE 4   ; Overflow
ISR_NOERRCODE 5   ; Bound Range Exceeded
ISR_NOERRCODE 6   ; Invalid Opcode
ISR_NOERRCODE 7   ; Device Not Available
ISR_ERRCODE   8   ; Double Fault
ISR_NOERRCODE 9   ; Coprocessor Segment Overrun
ISR_ERRCODE   10  ; Invalid TSS
ISR_ERRCODE   11  ; Segment Not Present
ISR_ERRCODE   12  ; Stack-Segment Fault
ISR_ERRCODE   13  ; General Protection Fault
ISR_ERRCODE   14  ; Page Fault
ISR_NOERRCODE 15  ; Reserved
ISR_NOERRCODE 16  ; x87 Floating-Point Exception
ISR_ERRCODE   17  ; Alignment Check
ISR_NOERRCODE 18  ; Machine Check
ISR_NOERRCODE 19  ; SIMD Floating-Point Exception
ISR_NOERRCODE 20  ; Virtualization Exception
ISR_ERRCODE   21  ; Control Protection Exception
ISR_NOERRCODE 22  ; Reserved
ISR_NOERRCODE 23  ; Reserved
ISR_NOERRCODE 24  ; Reserved
ISR_NOERRCODE 25  ; Reserved
ISR_NOERRCODE 26  ; Reserved
ISR_NOERRCODE 27  ; Reserved
ISR_NOERRCODE 28  ; Hypervisor Injection Exception
ISR_ERRCODE   29  ; VMM Communication Exception
ISR_ERRCODE   30  ; Security Exception
ISR_NOERRCODE 31  ; Reserved

; IRQ handlers (remapped to 32-47)
IRQ 32, 0   ; IRQ0 - PIT Timer
IRQ 33, 1   ; IRQ1 - Keyboard
IRQ 34, 2   ; IRQ2 - Cascade
IRQ 35, 3   ; IRQ3 - COM2
IRQ 36, 4   ; IRQ4 - COM1
IRQ 37, 5   ; IRQ5 - LPT2
IRQ 38, 6   ; IRQ6 - Floppy
IRQ 39, 7   ; IRQ7 - LPT1
IRQ 40, 8   ; IRQ8 - RTC
IRQ 41, 9   ; IRQ9 - Free
IRQ 42, 10  ; IRQ10 - Free
IRQ 43, 11  ; IRQ11 - Free
IRQ 44, 12  ; IRQ12 - PS2 Mouse
IRQ 45, 13  ; IRQ13 - FPU
IRQ 46, 14  ; IRQ14 - Primary ATA
IRQ 47, 15  ; IRQ15 - Secondary ATA

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

irq_common_stub:
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
  call irq_handler

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

  ; Clean up error code and IRQ number (16 bytes)
  add rsp, 16

  iretq

global idt_load
idt_load:
  lidt [rdi]
  ret
