#include "../inc/terminal.h"

static unsigned char *vga_buffer = (unsigned char *)0xB8000;
static int cursor_x = 0;
static int cursor_y = 0;
static unsigned char color = 0x0F;

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

void terminal_initialize(void) { terminal_clear(); }

void terminal_clear(void) {
  for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT * 2; i += 2) {
    vga_buffer[i] = ' ';
    vga_buffer[i + 1] = color;
  }
  cursor_x = 0;
  cursor_y = 0;
}
