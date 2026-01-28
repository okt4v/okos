#include "../inc/terminal.h"

// Declare this BEFORE kernel_main
static inline unsigned long get_stack_pointer(void) {
  unsigned long rsp;
  __asm__ volatile("mov %%rsp, %0" : "=r"(rsp));
  return rsp;
}

void kernel_main(unsigned long magic, unsigned long addr) {
  terminal_initialize();

  // Test basic printf
  terminal_printf("Hello from OKOS!\n\n");

  // Display memory addresses
  terminal_printf("Multiboot magic: 0x%x\n", magic);
  terminal_printf("Multiboot info at: %p\n", addr);
  terminal_printf("Kernel main at: %p\n", (unsigned long)kernel_main);
  terminal_printf("VGA buffer at: %p\n", 0xB8000UL);

  // Test colors
  terminal_printf("\n{0A}Green text{0F} normal ");
  terminal_printf("{0C}Red text{0F} normal\n");

  // Stack pointer
  terminal_printf("\n{0E}Stack pointer: {0F}%p\n", get_stack_pointer());

  while (1) {
    __asm__("hlt");
  }
}
