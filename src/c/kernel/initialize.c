#include "../inc/initialize.h"
#include "../inc/idt.h"
#include "../inc/keyboard.h"
#include "../inc/pic.h"
#include "../inc/terminal.h"

void system_initialize(void) {
  // Initialize terminal first (so we can print messages)
  terminal_initialize();

  // Print boot banner
  terminal_printf("{0B}=== OKOS 64-bit ==={0F}\n\n");

  // Install Interrupt Descriptor Table
  idt_install();

  // Initialize and remap PIC (map IRQs to interrupts 32-47)
  pic_remap(0x20, 0x28);

  // Install keyboard driver (before enabling interrupts)
  keyboard_install();

  // Enable hardware interrupts
  __asm__ volatile("sti");

  // Ready message
  terminal_printf("\n{0E}Keyboard ready. Start typing!{0F}\n");
  terminal_printf("> ");
}
