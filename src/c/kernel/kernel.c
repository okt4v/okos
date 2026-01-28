#include "../inc/idt.h"
#include "../inc/terminal.h"

void kernel_main(unsigned long magic, unsigned long addr) {
  terminal_initialize();

  while (1) {
    __asm__("hlt");
  }
}
