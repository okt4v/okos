#include "../inc/keyboard.h"
#include "../inc/idt.h"
#include "../inc/pic.h"
#include "../inc/terminal.h"

// US QWERTY scancode set 1 translation table (lowercase)
static const char scancode_to_ascii[] = {
    0,    0,    '1',  '2', '3', '4', '5', '6', '7', '8', '9', '0',  '-',
    '=',  '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o',  'p',
    '[',  ']',  '\n', 0,   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k',  'l',
    ';',  '\'', '`',  0,   '\\','z', 'x', 'c', 'v', 'b', 'n', 'm',  ',',
    '.',  '/',  0,    '*', 0,   ' ', 0,   0,   0,   0,   0,   0,    0,
    0,    0,    0,    0,   0,   0,   0,   0,   0,   '-', 0,   0,    0,
    '+',  0,    0,    0,   0,   0,   0,   0,   0,   0,   0,   0};

// Shifted characters
static const char scancode_to_ascii_shift[] = {
    0,    0,    '!',  '@', '#', '$', '%', '^', '&', '*', '(', ')',  '_',
    '+',  '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O',  'P',
    '{',  '}',  '\n', 0,   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K',  'L',
    ':',  '"',  '~',  0,   '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M',  '<',
    '>',  '?',  0,    '*', 0,   ' ', 0,   0,   0,   0,   0,   0,    0,
    0,    0,    0,    0,   0,   0,   0,   0,   0,   '-', 0,   0,    0,
    '+',  0,    0,    0,   0,   0,   0,   0,   0,   0,   0,   0};

static uint8_t shift_pressed = 0;
static uint8_t caps_lock = 0;

void keyboard_handler(void) {
  uint8_t scancode = inb(KEYBOARD_DATA_PORT);

  // Handle key release (scancode with high bit set)
  if (scancode & 0x80) {
    scancode &= 0x7F;
    // Left shift (0x2A) or right shift (0x36) released
    if (scancode == 0x2A || scancode == 0x36) {
      shift_pressed = 0;
    }
  } else {
    // Key press
    // Left shift (0x2A) or right shift (0x36) pressed
    if (scancode == 0x2A || scancode == 0x36) {
      shift_pressed = 1;
      return;
    }

    // Caps Lock (0x3A)
    if (scancode == 0x3A) {
      caps_lock = !caps_lock;
      return;
    }

    // Translate scancode to ASCII
    char c = 0;
    if (scancode < sizeof(scancode_to_ascii)) {
      if (shift_pressed) {
        c = scancode_to_ascii_shift[scancode];
      } else {
        c = scancode_to_ascii[scancode];
      }

      // Apply caps lock to letters
      if (caps_lock && c >= 'a' && c <= 'z') {
        c = c - 'a' + 'A';
      } else if (caps_lock && c >= 'A' && c <= 'Z') {
        c = c - 'A' + 'a';
      }

      if (c) {
        terminal_putchar(c);
      }
    }
  }
}

void keyboard_install(void) {
  // Register keyboard handler for IRQ1
  irq_install_handler(1, keyboard_handler);

  // Enable IRQ1 on PIC
  pic_clear_mask(1);

  terminal_printf("{0A}[Keyboard]{0F} PS/2 keyboard driver installed\n");
}
