#ifndef PIC_H
#define PIC_H

#include <stdint.h>

// PIC I/O ports
#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

// PIC commands
#define PIC_EOI 0x20 // End of Interrupt

// I/O port operations
static inline void outb(uint16_t port, uint8_t value) {
  __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
  uint8_t ret;
  __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}

// Initialize and remap the PIC
void pic_remap(uint8_t offset1, uint8_t offset2);

// Send End of Interrupt signal
void pic_send_eoi(uint8_t irq);

// Enable/disable IRQ lines
void pic_set_mask(uint8_t irq);
void pic_clear_mask(uint8_t irq);

#endif
