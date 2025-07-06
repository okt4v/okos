; Bootloader for the okos kernel

section .text:
  global _start
  extern kernel_main

_start:
  cli ;clear interupts
  xor ax, ax
  mov ds, ax
  mov es, ax
  mov ss, ax
  mov sp, 0x80000 ;stack pointer

  call kernel_main ;call kernel_main funtion from cpp file

hang:
  jmp hang

