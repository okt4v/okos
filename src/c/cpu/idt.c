#include "../inc/idt.h"
#include "../inc/terminal.h"
#include <stdint.h>

struct idt_entry {
  uint16_t offset_low;
  uint16_t selector;
  uint8_t ist;
  uint8_t type_attr;
  uint16_t offset_mid;
  uint16_t offset_high;
  uint32_t zero;
} __attribute__((packed));

struct idt_ptr {
  uint16_t limit;
  uint64_t base;
} __attribute__((packed));

static struct idt_entry idt[256];
static struct idt_ptr idtp;

void idt_set_gate(uint8_t num, uint64_t handler, uint16_t selector,
                  uint8_t flags) {
  idt[num].offset_low = handler & 0xFFFF;
  idt[num].offset_mid = (handler >> 16) & 0xFFFf;
  idt[num].offset_high = (handler >> 32) & 0xFFFFFFFF;

  idt[num].selector = selector;
  idt[num].ist = 0;
  idt[num].type_attr = flags;
  idt[num].zero = 0;
}

extern void idt_load(uint64_t);

void idt_install(void) {
  idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
  idtp.base = (uint64_t)&idt;

  for (int i = 0; i < 256; i++) {
    idt_set_gate(i, 0, 0, 0);
  }

  extern void isr0(void);
  extern void isr1(void);
  extern void isr2(void);
  extern void isr3(void);
  extern void isr4(void);
  extern void isr5(void);
  extern void isr6(void);
  extern void isr7(void);
  extern void isr8(void);
  extern void isr13(void);
  extern void isr14(void);

  // Register exception handlers (0x8E = present, ring 0, 64-bit interrupt gate)
  idt_set_gate(0, (uint64_t)isr0, 0x08, 0x8E);
  idt_set_gate(1, (uint64_t)isr1, 0x08, 0x8E);
  idt_set_gate(2, (uint64_t)isr2, 0x08, 0x8E);
  idt_set_gate(3, (uint64_t)isr3, 0x08, 0x8E);
  idt_set_gate(4, (uint64_t)isr4, 0x08, 0x8E);
  idt_set_gate(5, (uint64_t)isr5, 0x08, 0x8E);
  idt_set_gate(6, (uint64_t)isr6, 0x08, 0x8E);
  idt_set_gate(7, (uint64_t)isr7, 0x08, 0x8E);
  idt_set_gate(8, (uint64_t)isr8, 0x08, 0x8E);
  idt_set_gate(13, (uint64_t)isr13, 0x08, 0x8E);
  idt_set_gate(14, (uint64_t)isr14, 0x08, 0x8E);

  idt_load((uint64_t)&idtp);

  terminal_printf("{0A}[IDT]{0F} Interrupt Descriptor Table installed\n");
}

void isr_handler(uint64_t interrupt_number, uint64_t error_code) {
  terminal_printf("\n{0C}*** EXCEPTION ***{0F}\n");
  terminal_printf("Interrupt: %d (0x%x)\n", interrupt_number, interrupt_number);
  terminal_printf("Error code: 0x%x\n", error_code);

  const char *exception_messages[] = {"Division By Zero",
                                      "Debug",
                                      "Non Maskable Interrupt",
                                      "Breakpoint",
                                      "Overflow",
                                      "Bound Range Exceeded",
                                      "Invalid Opcode",
                                      "Device Not Available",
                                      "Double Fault",
                                      "Coprocessor Segment Overrun",
                                      "Invalid TSS",
                                      "Segment Not Present",
                                      "Stack-Segment Fault",
                                      "General Protection Fault",
                                      "Page Fault",
                                      "Reserved",
                                      "x87 Floating-Point Exception"};
  if (interrupt_number < 17) {
    terminal_printf("Description: %s\n", exception_messages[interrupt_number]);
  }

  terminal_printf("\n{0C}System Halted{0F}\n");

  while (1) {
    __asm__ volatile("cli; hlt");
  }
}
