#include "../inc/pic.h"
#include "../inc/terminal.h"

static inline void io_wait(void) {
  // Port 0x80 is used for POST codes, writing to it causes a delay
  outb(0x80, 0);
}

// Remap the PIC to avoid conflicts with CPU exceptions
void pic_remap(uint8_t offset1, uint8_t offset2) {

  // Start initialization sequence (ICW1)
  outb(PIC1_COMMAND, 0x11);
  io_wait();
  outb(PIC2_COMMAND, 0x11);
  io_wait();

  // Set vector offsets (ICW2)
  outb(PIC1_DATA, offset1);
  io_wait();
  outb(PIC2_DATA, offset2);
  io_wait();

  // Tell master PIC about slave at IRQ2 (ICW3)
  outb(PIC1_DATA, 0x04);
  io_wait();
  // Tell slave PIC its cascade identity (ICW3)
  outb(PIC2_DATA, 0x02);
  io_wait();

  // Set 8086 mode (ICW4)
  outb(PIC1_DATA, 0x01);
  io_wait();
  outb(PIC2_DATA, 0x01);
  io_wait();

  // Mask all IRQs initially (we'll enable them as needed)
  outb(PIC1_DATA, 0xFF);
  outb(PIC2_DATA, 0xFF);

  terminal_printf("{0A}[PIC]{0F} Remapped to IRQ %d-%d and %d-%d\n", offset1,
                  offset1 + 7, offset2, offset2 + 7);
}

// Send End of Interrupt signal
void pic_send_eoi(uint8_t irq) {
  if (irq >= 8) {
    outb(PIC2_COMMAND, PIC_EOI);
  }
  outb(PIC1_COMMAND, PIC_EOI);
}

// Disable IRQ line (set mask bit)
void pic_set_mask(uint8_t irq) {
  uint16_t port;
  uint8_t value;

  if (irq < 8) {
    port = PIC1_DATA;
  } else {
    port = PIC2_DATA;
    irq -= 8;
  }
  value = inb(port) | (1 << irq);
  outb(port, value);
}

// Enable IRQ line (clear mask bit)
void pic_clear_mask(uint8_t irq) {
  uint16_t port;
  uint8_t value;

  if (irq < 8) {
    port = PIC1_DATA;
  } else {
    port = PIC2_DATA;
    irq -= 8;
  }
  value = inb(port) & ~(1 << irq);
  outb(port, value);
}
