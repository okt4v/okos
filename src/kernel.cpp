#include <stddef.h>
#include <stdint.h>

const size_t vga_width = 80;
const size_t vga_height = 25;
uint16_t *const vga_memory = (uint16_t *)0xb8000;

const uint8_t default_color = 0x07;

size_t terminal_row = 0;
size_t terminal_column = 0;

void clear_screen() {
  uint16_t blank = ((uint16_t)default_color << 8) | ' ';
  for (size_t i = 0; i < vga_width * vga_height; ++i) {
    vga_memory[i] = blank;
  }
  terminal_row = 0;
  terminal_column = 0;
}

void putchar_at(char c, size_t x, size_t y) {
  const size_t index = y * vga_width + x;
  vga_memory[index] = ((uint16_t)default_color << 8) | c;
}

void print(const char *str) {
  for (size_t i = 0; str[i] != '\0'; ++i) {
    if (str[i] == '\n') {
      terminal_column = 0;
      terminal_row++;
    } else {
      putchar_at(str[i], terminal_column, terminal_row);
      terminal_column++;
      if (terminal_column >= vga_width) {
        terminal_column = 0;
        terminal_row++;
      }
    }
  }
}

extern "C" void kernel_main() {
  clear_screen();
  print("This is a test");

  while (1) {
    __asm__("hlt");
  }
}
