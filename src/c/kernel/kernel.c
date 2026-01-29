#include "../inc/initialize.h"

void kernel_main(void) {
  system_initialize();

  while (1) {
    __asm__ volatile("hlt");
  }
}
