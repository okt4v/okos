#include "../inc/initialize.h"

void kernel_main(void) {
  // Initialize all system components
  system_initialize();

  // Main kernel loop - halt until interrupt
  while (1) {
    __asm__ volatile("hlt");
  }
}
