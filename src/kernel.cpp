// TODO: create a sleep funtion to delay events
// TODO: keyboard input (i think i need to write some more assembly)
// (remindercheck deepseek for help)
// TODO: add input reading capabilities
// TODO: add funtional commands

#include <stddef.h>
#include <stdint.h>

const size_t vga_width = 80;
const size_t vga_height = 25;
uint16_t *const vga_memory = (uint16_t *)0xb8000;

const uint8_t default_color = 0x8A;

size_t terminal_row = 0;
size_t terminal_column = 0;
bool shift_pressed = false;
bool caps_lock = false;
bool alt_gr_pressed = false;

void clear_screen() {
  uint16_t blank = ((uint16_t)default_color << 8) | ' ';
  for (size_t i = 0; i < vga_width * vga_height; ++i) {
    vga_memory[i] = blank;
  }
  terminal_row = 0;
  terminal_column = 0;
}

void putchar_at(uint8_t c, size_t x, size_t y) {
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

// German QWERTZ scancode map (non-shifted) - CP437 hex values
static const uint8_t scancodes[] = {
    0,           0,           '1', '2', '3',        '4',        '5',
    '6',         '7',         '8', '9', '0',        0xE1 /*ß*/, 0x60 /*´*/,
    0x08 /*\b*/, 0x09 /*\t*/, 'q', 'w', 'e',        'r',        't',
    'z',         'u',         'i', 'o', 'p',        0x81 /*ü*/, '+',
    0x0A /*\n*/, 0,           'a', 's', 'd',        'f',        'g',
    'h',         'j',         'k', 'l', 0x94 /*ö*/, 0x84 /*ä*/, '#',
    0,           '<',         'y', 'x', 'c',        'v',        'b',
    'n',         'm',         ',', '.', '-',        0,          '*',
    0,           ' ',         0,   0,   0,          0,          0,
    0,           0,           0,   0,   0,          0,          0,
    0,           '7',         '8', '9', '-',        '4',        '5',
    '6',         '+',         '1', '2', '3',        '0',        '.'};

// German QWERTZ scancode map (shifted) - CP437 hex values
static const uint8_t shift_scancodes[] = {
    0,   0,    '!', '"',        0x15 /*§*/, '$',  '%',  '&', '/',
    '(', ')',  '=', '?',        '`',        0x08, 0x09, 'Q', 'W',
    'E', 'R',  'T', 'Z',        'U',        'I',  'O',  'P', 0x9A /*Ü*/,
    '*', 0x0A, 0,   'A',        'S',        'D',  'F',  'G', 'H',
    'J', 'K',  'L', 0x99 /*Ö*/, 0x8E /*Ä*/, '\'', 0,    '>', 'Y',
    'X', 'C',  'V', 'B',        'N',        'M',  ';',  ':', '_',
    0,   '*',  0,   ' ',        0,          0,    0,    0,   0,
    0,   0,    0,   0,          0,          0,    0,    0,   '7',
    '8', '9',  '-', '4',        '5',        '6',  '+',  '1', '2',
    '3', '0',  '.'};

// AltGr mappings (ISO Level 3) - CP437 hex values
static const uint8_t altgr_scancodes[] = {
    0,          0,          0xFA /*¹*/, 0xFD /*²*/, 0xFC /*³*/, 0xAC /*¼*/,
    0xAB /*½*/, 0xAF /*¬*/, '{',        '[',        ']',        '}',
    '\\',       0,          0x08,       0,          '@',        0x9D /*ł*/,
    0xEE /*€*/, 0xF4 /*¶*/, 0x9F /*ŧ*/, 0x1F /*←*/, 0x1E /*↓*/, 0x1D /*→*/,
    0xEB /*ø*/, 0xFE /*þ*/, '~',        '~',        0x0A,       0,
    0x91 /*æ*/, 0x9B /*ſ*/, 0x92 /*ð*/, 0x90 /*đ*/, 0x95 /*ŋ*/, 0xA5 /*ħ*/,
    'j',        0x8B /*ĸ*/, 0x9D /*ł*/, 0xEF /*˝*/, '^',        0xA6 /*’*/,
    0,          '|',        0xAE /*»*/, 0xAE /*«*/, 0x9B /*¢*/, '"',
    '"',        'n',        0xE6 /*µ*/, 0xF9 /*·*/, 0x85 /*…*/, 0x96 /*–*/,
    0,          0,          0,          ' ',        0,          0,
    0,          0,          0,          0,          0,          0,
    0,          0,          0,          0,          0,          '7',
    '8',        '9',        '-',        '4',        '5',        '6',
    '+',        '1',        '2',        '3',        '0',        '.'};

extern "C" void keyboard_handler(uint8_t scancode) {
  switch (scancode) {
  case 0x2A:
  case 0x36: // Left / Right shift pressed
    shift_pressed = true;
    return;
  case 0xAA:
  case 0xB6:
    shift_pressed = false; // Left / Right shift released
    return;
  case 0x38: // AltGr pressed
    alt_gr_pressed = true;
    return;
  case 0xB8: // AltGr released
    alt_gr_pressed = false;
    return;
  case 0x3A: // capslock
    caps_lock = !caps_lock;
    return;
  }

  if (scancode & 0x80)
    return;

  if (scancode < sizeof(scancodes)) {
    uint8_t c = 0;
    if (alt_gr_pressed && altgr_scancodes[scancode] != 0) {
      c = altgr_scancodes[scancode];
    } else if (shift_pressed ^ caps_lock) {
      c = shift_scancodes[scancode];
    } else {
      c = scancodes[scancode];
    }

    if (c != 0) {
      if (c == '\b') { // backspace
        if (terminal_column > 0) {
          terminal_column--;
          putchar_at(' ', terminal_column, terminal_row);
        }
      } else if (c == '\n') { // enter
        print("\nOKOS>");     // print new prompt to screen
      } else {
        putchar_at(c, terminal_column, terminal_row);
        terminal_column++;
        if (terminal_column > vga_width) {
          terminal_column = 0;
          terminal_row++;
        }
      }
    }
  }
}

void banner() {
  print("\n\n\n\n\n\n\n\n\n");
  print("                            ");
  print(" ___       ___   ___\n");
  print("                            ");
  print("/   \\ | / /   \\ /   \\\n");
  print("                            ");
  print("|   | |/  |   | |___\n");
  print("                            ");
  print("|   | |\\  |   |     \\\n");
  print("                            ");
  print("\\___/ | \\ \\___/ \\___|\n");
  print("\n                        ");
  print("https://github.com/okt4v/okos\n");
}

extern "C" void kernel_main() {
  clear_screen();
  print("Welcome to OKOS!\n");
  print("This OS is still under development please be patient...\n");
  print("\n");
  print("View the progress at https://github.com/okt4v/okos\n");
  clear_screen();
  banner();
  while (1) {
    __asm__("hlt");
  }
}
