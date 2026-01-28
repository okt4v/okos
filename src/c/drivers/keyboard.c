#include "../inc/keyboard.h"
#include "../inc/idt.h"
#include "../inc/pic.h"
#include "../inc/shell.h"
#include "../inc/terminal.h"

// Keyboard layout enum
typedef enum { LAYOUT_US, LAYOUT_DE } keyboard_layout_t;

static keyboard_layout_t current_layout = LAYOUT_US;

// US QWERTY scancode set 1 translation table (lowercase)
static const char us_scancode_to_ascii[] = {
    0,   0,    '1',  '2', '3',  '4', '5', '6', '7', '8', '9', '0', '-',
    '=', '\b', '\t', 'q', 'w',  'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
    '[', ']',  '\n', 0,   'a',  's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
    ';', '\'', '`',  0,   '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',',
    '.', '/',  0,    '*', 0,    ' ', 0,   0,   0,   0,   0,   0,   0,
    0,   0,    0,    0,   0,    0,   0,   0,   0,   '-', 0,   0,   0,
    '+', 0,    0,    0,   0,    0,   0,   0,   0,   0,   0,   0};

// US shifted characters
static const char us_scancode_to_ascii_shift[] = {
    0,   0,    '!',  '@', '#', '$', '%', '^', '&', '*', '(', ')', '_',
    '+', '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
    '{', '}',  '\n', 0,   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
    ':', '"',  '~',  0,   '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<',
    '>', '?',  0,    '*', 0,   ' ', 0,   0,   0,   0,   0,   0,   0,
    0,   0,    0,    0,   0,   0,   0,   0,   0,   '-', 0,   0,   0,
    '+', 0,    0,    0,   0,   0,   0,   0,   0,   0,   0,   0};

// German (de-latin1) scancode set 1 translation table (lowercase)
static const char de_scancode_to_ascii[] =
    {
        0,    0,    '1',  '2',  '3', '4',  '5', '6', '7', '8',
        '9',  '0',  0xDF, // ß
        '\'', '\b', '\t', 'q',  'w', 'e',  'r', 't', 'z', 'u',
        'i',  'o',  'p',  0xFC, '+', '\n', 0,   'a', 's', 'd',
        'f',  'g',  'h',  'j',  'k', 'l', // ü
        0xF6, 0xE4, '^',  0,    '#', 'y',  'x', 'c', 'v', 'b',
        'n',  'm',  ',', // ö, ä
        '.',  '-',  0,    '*',  0,   ' ',  0,   0,   0,   0,
        0,    0,    0,    0,    0,   0,    0,   0,   0,   0,
        0,    0,    '-',  0,    0,   0,    '+', 0,   0,   0,
        0,    0,    0,    0,    0,   0,    '<', 0}; // < key (scancode 86)

// German shifted characters
static const char de_scancode_to_ascii_shift[] = {
    0,    0,    '!',  '"', 0xA7, '$', '%', '&', '/', '(', ')', '=', '?', // §
    '`',  '\b', '\t', 'Q', 'W',  'E', 'R', 'T', 'Z', 'U', 'I', 'O', 'P', 0xDC,
    '*',  '\n', 0,    'A', 'S',  'D', 'F', 'G', 'H', 'J', 'K', 'L',      // Ü
    0xD6, 0xC4, 0xB0, 0,   '\'', 'Y', 'X', 'C', 'V', 'B', 'N', 'M', ';', // Ö,
                                                                         // Ä, °
    ':',  '_',  0,    '*', 0,    ' ', 0,   0,   0,   0,   0,   0,   0,   0,
    0,    0,    0,    0,   0,    0,   0,   0,   '-', 0,   0,   0,   '+', 0,
    0,    0,    0,    0,   0,    0,   0,   0,   '>', 0}; // > key (scancode 86)

// German AltGr characters (right Alt modifier)
static const char de_scancode_to_ascii_altgr[] = {
    0,   0, 0, 0, 0, 0, 0, '{', '[', ']', '}', '\\', 0,    0, '\b', '\t',
    '@', 0, 0, 0, 0, 0, 0, 0,   0,   0,   0,   '~',  '\n', 0, 0,    0,
    0,   0, 0, 0, 0, 0, 0, 0,   0,   0,   0,   0,    0,    0, 0,    0,
    0,   0, 0, 0, 0, 0, 0, '*', 0,   ' ', 0,   0,    0,    0, 0,    0,
    0,   0, 0, 0, 0, 0, 0, 0,   0,   0,   '-', 0,    0,    0, '+',  0,
    0,   0, 0, 0, 0, 0, 0, 0,   '|', 0}; // | key (scancode 86)

static uint8_t shift_pressed = 0;
static uint8_t caps_lock = 0;
static uint8_t altgr_pressed = 0;

void keyboard_handler(void) {
  static uint8_t e0_prefix = 0;
  uint8_t scancode = inb(KEYBOARD_DATA_PORT);

  // Check for E0 prefix (extended keys)
  if (scancode == 0xE0) {
    e0_prefix = 1;
    return;
  }

  // Handle key release (scancode with high bit set)
  if (scancode & 0x80) {
    scancode &= 0x7F;
    // Left shift (0x2A) or right shift (0x36) released
    if (scancode == 0x2A || scancode == 0x36) {
      shift_pressed = 0;
    }
    // Right Alt (AltGr) released (0x38 with E0 prefix)
    if (scancode == 0x38 && e0_prefix) {
      altgr_pressed = 0;
    }
    e0_prefix = 0;
  } else {
    // Key press
    // Left shift (0x2A) or right shift (0x36) pressed
    if (scancode == 0x2A || scancode == 0x36) {
      shift_pressed = 1;
      e0_prefix = 0;
      return;
    }

    // Right Alt (AltGr) pressed (0x38 with E0 prefix)
    if (scancode == 0x38 && e0_prefix) {
      altgr_pressed = 1;
      e0_prefix = 0;
      return;
    }

    // Left Alt pressed (ignore for now)
    if (scancode == 0x38 && !e0_prefix) {
      e0_prefix = 0;
      return;
    }

    // Caps Lock (0x3A)
    if (scancode == 0x3A) {
      caps_lock = !caps_lock;
      e0_prefix = 0;
      return;
    }

    // Translate scancode to ASCII based on current layout
    char c = 0;
    const char *layout_normal = (current_layout == LAYOUT_DE)
                                    ? de_scancode_to_ascii
                                    : us_scancode_to_ascii;
    const char *layout_shift = (current_layout == LAYOUT_DE)
                                   ? de_scancode_to_ascii_shift
                                   : us_scancode_to_ascii_shift;

    if (scancode < sizeof(us_scancode_to_ascii)) {
      // Check AltGr first (only for German layout)
      if (altgr_pressed && current_layout == LAYOUT_DE) {
        c = de_scancode_to_ascii_altgr[scancode];
      } else if (shift_pressed) {
        c = layout_shift[scancode];
      } else {
        c = layout_normal[scancode];
      }

      e0_prefix = 0;

      // Apply caps lock to letters
      if (caps_lock && c >= 'a' && c <= 'z') {
        c = c - 'a' + 'A';
      } else if (caps_lock && c >= 'A' && c <= 'Z') {
        c = c - 'A' + 'a';
      }

      if (c) {
        shell_input(c);
      }
    }
  }
}

void keyboard_install(void) {
  // Register keyboard handler for IRQ1
  irq_install_handler(1, keyboard_handler);

  // Enable IRQ1 on PIC
  pic_clear_mask(1);

  terminal_printf(
      "{0A}[Keyboard]{0F} PS/2 keyboard driver installed (US layout)\n");
}

void keyboard_set_layout(const char *layout_name) {
  if (layout_name[0] == 'u' && layout_name[1] == 's' &&
      layout_name[2] == '\0') {
    current_layout = LAYOUT_US;
    terminal_printf("{0A}Keyboard layout set to: US{0F}\n");
  } else if (layout_name[0] == 'd' && layout_name[1] == 'e' &&
             layout_name[2] == '\0') {
    current_layout = LAYOUT_DE;
    terminal_printf("{0A}Keyboard layout set to: German (de-latin1){0F}\n");
  } else {
    terminal_printf("{0C}Unknown layout: %s{0F}\n", layout_name);
    terminal_printf("Available layouts: us, de\n");
  }
}

const char *keyboard_get_layout(void) {
  return (current_layout == LAYOUT_DE) ? "de" : "us";
}
