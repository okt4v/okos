#include "../inc/terminal.h"

void kernel_main(unsigned long magic, unsigned long addr) {
  (void)magic;
  (void)addr;

  terminal_initialize();

  terminal_putchar('T');
  terminal_putchar('E');
  terminal_putchar('S');
  terminal_putchar('T');

  while (1) {
    __asm__("hlt");
  }
}
