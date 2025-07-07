#include <cstddef>
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
  // continue with this tomorow and add print function
  /*
  void putchar_at(char c, size_t x, size_t y) {
    const size_t index = y * vga_width + x;
    vga_memory[index] = ((uint16_t)default_color << 8) | c;
  }

  void print(const char* str) {
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

      if (terminal_row >= vga_height) {
        // No scrolling implemented, so just stop at bottom
        break;
      }
    }
  }
  */
}

extern "C" void kernel_main() {
  clear_screen();

  vga_memory[0] = 'W';
  vga_memory[1] = 0x0F;
  vga_memory[2] = 'e';
  vga_memory[3] = 0x0F;
  vga_memory[4] = 'l';
  vga_memory[5] = 0x0F;
  vga_memory[6] = 'c';
  vga_memory[7] = 0x0F;
  vga_memory[8] = 'o';
  vga_memory[9] = 0x0F;
  vga_memory[10] = 'm';
  vga_memory[11] = 0x0F;
  vga_memory[12] = 'e';
  vga_memory[13] = 0x0F;
  vga_memory[14] = ' ';
  vga_memory[15] = 0x0F;
  vga_memory[16] = 't';
  vga_memory[17] = 0x0F;
  vga_memory[18] = 'o';
  vga_memory[19] = 0x0F;
  vga_memory[20] = ' ';
  vga_memory[21] = 0x0F;
  vga_memory[22] = 'O';
  vga_memory[23] = 0x0F;
  vga_memory[24] = 'K';
  vga_memory[25] = 0x0F;
  vga_memory[26] = 'O';
  vga_memory[27] = 0x0F;
  vga_memory[28] = 'S';
  vga_memory[29] = 0x0F;
  vga_memory[30] = '!';
  vga_memory[31] = 0x0F;

  while (1) {
    __asm__("hlt");
  }
}
