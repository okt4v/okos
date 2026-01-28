#include "../inc/terminal.h"

static unsigned char *vga_buffer = (unsigned char *)0xB8000;
static int cursor_x = 0;
static int cursor_y = 0;
static unsigned char color = 0x0F;

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

void terminal_initialize(void) {
  terminal_clear();

  terminal_writestring("Welcome to ");
  terminal_setcolor(COLOR_LIGHT_GREEN);
  terminal_writestring("OKOS");
  terminal_setcolor(COLOR_WHITE);
  terminal_writestring("!\n\n");
}

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

void terminal_writeint(int num) {
  if (num == 0) {
    terminal_putchar('0');
    return;
  }

  if (num < 0) {
    terminal_putchar('-');
    num = -num;
  }

  char buffer[12];
  int i = 0;

  while (num > 0) {
    buffer[i++] = '0' + (num % 10);
    num /= 10;
  }

  while (i > 0) {
    terminal_putchar(buffer[--i]);
  }
}

void terminal_writestring(const char *str) {
  while (*str) {
    terminal_putchar(*str);
    str++;
  }
}

void terminal_writehex(unsigned long num) {
  terminal_writestring("0x");

  if (num == 0) {
    terminal_putchar('0');
    return;
  }

  char buffer[17];
  int i = 0;

  while (num > 0) {
    int digit = num % 16;
    buffer[i++] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
    num /= 16;
  }

  while (i > 0) {
    terminal_putchar(buffer[--i]);
  }
}
