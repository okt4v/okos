#include "../inc/terminal.h"

#include <stdarg.h>

static unsigned char *vga_buffer = (unsigned char *)0xB8000;
static int cursor_x = 0;
static int cursor_y = 0;
static unsigned char color = 0x0F;

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

void terminal_initialize(void) {
  terminal_clear();
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

static char digit_to_hex(int digit) {
  if (digit < 10)
    return '0' + digit;
  return 'A' + (digit - 10);
}

static void print_number(unsigned long num, int base, int is_signed) {
  if (is_signed && (long)num < 0) {
    terminal_putchar('-');
    num = -(long)num;
  }

  if (num == 0) {
    terminal_putchar('0');
    return;
  }

  char buffer[32];
  int i = 0;

  while (num > 0) {
    int digit = num % base;
    buffer[i++] = digit_to_hex(digit);
    num /= base;
  }

  while (i > 0) {
    terminal_putchar(buffer[--i]);
  }
}

static unsigned char parse_color_code(const char *str) {
  unsigned char result = 0;

  if (str[0] >= '0' && str[0] <= '9')
    result = (str[0] - '0') << 4;
  else if (str[0] >= 'A' && str[0] <= 'F')
    result = (str[0] - 'A' + 10) << 4;
  else if (str[0] >= 'a' && str[0] <= 'f')
    result = (str[0] - 'a' + 10) << 4;

  if (str[1] >= '0' && str[1] <= '9')
    result |= (str[1] - '0');
  else if (str[1] >= 'A' && str[1] <= 'F')
    result |= (str[1] - 'A' + 10);
  else if (str[1] >= 'a' && str[1] <= 'f')
    result |= (str[1] - 'a' + 10);

  return result;
}

void terminal_printf(const char *format, ...) {
  va_list args;
  va_start(args, format);

  unsigned char saved_color = color;

  for (int i = 0; format[i] != '\0'; i++) {
    if (format[i] == '%' && format[i + 1] != '\0') {
      i++;

      switch (format[i]) {
      case 'd': {
        int val = va_arg(args, int);
        print_number(val, 10, 1);
        break;
      }
      case 'u': {
        unsigned int val = va_arg(args, unsigned int);
        print_number(val, 10, 0);
        break;
      }
      case 'x': {
        unsigned int val = va_arg(args, unsigned int);
        print_number(val, 16, 0); // lowcase hex
        break;
      }
      case 'X': {
        unsigned int val = va_arg(args, unsigned int);
        print_number(
            val, 16,
            0); // uppercase hex in future, same as lowcase for now though
        break;
      }
      case 'p': {
        terminal_writestring("0x");
        unsigned long val = va_arg(args, unsigned long);
        print_number(val, 16, 0);
        break;
      }
      case 's': {
        char *str = va_arg(args, char *);
        if (str == 0) {
          terminal_writestring("(null)");
        } else {
          terminal_writestring(str);
        }
        break;
      }
      case 'c': {
        char c = (char)va_arg(args, int);
        terminal_putchar(c);
        break;
      }
      case '%': {
        terminal_putchar('%');
        break;
      }
      default:
        terminal_putchar('%');
        terminal_putchar(format[i]);
        break;
      }
    } else if (format[i] == '{' && format[i + 1] != '\0' &&
               format[i + 2] != '\0' && format[i + 3] == '}') {
      unsigned char new_color = parse_color_code(&format[i + 1]);
      terminal_setcolor(new_color);
      i += 3;
    } else {
      terminal_putchar(format[i]);
    }
  }
  terminal_setcolor(saved_color);
  va_end(args);
}
