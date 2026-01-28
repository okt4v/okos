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

void terminal_setcolor(unsigned char new_color) { color = new_color; }

void terminal_putchar(char c) {
  if (c == '\n') {
    cursor_x = 0;
    cursor_y++;
  } else if (c == '\t') {
    cursor_x = (cursor_y + 4) & ~(4 - 1);
  } else {
    int index = (cursor_y * VGA_WIDTH + cursor_x) * 2;
    vga_buffer[index] = c;
    vga_buffer[index + 1] = color;
    cursor_x++;
  }

  if (cursor_x >= VGA_WIDTH) {
    cursor_x = 0;
    cursor_y++;
  }

  if (cursor_y >= VGA_HEIGHT) {
    terminal_scroll();
    cursor_y = VGA_HEIGHT - 1;
  }
}

void terminal_scroll(void) {
  for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH * 2; i++) {
    vga_buffer[i] = vga_buffer[i + VGA_WIDTH * 2];
  }

  for (int i = (VGA_HEIGHT - 1) * VGA_WIDTH * 2; i < VGA_HEIGHT * VGA_WIDTH * 2;
       i += 2) {
    vga_buffer[i] = ' ';
    vga_buffer[i + 1] = color;
  }
}
