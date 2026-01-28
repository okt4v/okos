#ifndef IDT_H
#define IDT_H

#include <stdint.h>

void idt_install(void);
void idt_set_gate(uint8_t num, uint64_t handler, uint16_t selector,
                  uint8_t flags);
void isr_handler(uint64_t interrupt_number, uint64_t error_code);
void irq_handler(uint64_t irq_number);
void irq_install_handler(uint8_t irq, void (*handler)(void));

#endif
