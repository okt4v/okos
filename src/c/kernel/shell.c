#include "../inc/shell.h"
#include "../inc/keyboard.h"
#include "../inc/terminal.h"
#include <stddef.h>

// Command buffer
static char command_buffer[SHELL_BUFFER_SIZE];
static uint16_t buffer_pos = 0;

// Simple string functions
static size_t strlen(const char *str) {
  size_t len = 0;
  while (str[len])
    len++;
  return len;
}

static int strcmp(const char *s1, const char *s2) {
  while (*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return *(unsigned char *)s1 - *(unsigned char *)s2;
}

static int strncmp(const char *s1, const char *s2, size_t n) {
  while (n && *s1 && (*s1 == *s2)) {
    s1++;
    s2++;
    n--;
  }
  if (n == 0)
    return 0;
  return *(unsigned char *)s1 - *(unsigned char *)s2;
}

static void strcpy(char *dest, const char *src) {
  while ((*dest++ = *src++))
    ;
}

// Command implementations
static void cmd_help(void) {
  terminal_printf("\n{0B}OKOS Shell Commands:{0F}\n");
  terminal_printf("  {0E}help{0F}         - Show this help message\n");
  terminal_printf("  {0E}clear{0F}        - Clear the screen\n");
  terminal_printf("  {0E}echo{0F} TEXT    - Print text to the screen\n");
  terminal_printf("  {0E}keymap{0F} [MAP] - Show or set keyboard layout (us, de)\n");
  terminal_printf("  {0E}about{0F}        - About this OS\n");
  terminal_printf("\n");
}

static void cmd_clear(void) {
  terminal_clear();
}

static void cmd_echo(char *args) {
  if (args && *args) {
    terminal_printf("%s\n", args);
  } else {
    terminal_printf("\n");
  }
}

static void cmd_about(void) {
  terminal_printf("\n{0B}OKOS{0F} - 64-bit Operating System\n");
  terminal_printf("Built from scratch with:\n");
  terminal_printf("  - Multiboot2 bootloader\n");
  terminal_printf("  - 64-bit long mode\n");
  terminal_printf("  - VGA text mode driver\n");
  terminal_printf("  - IDT with 48 interrupt handlers\n");
  terminal_printf("  - PIC remapping\n");
  terminal_printf("  - PS/2 keyboard driver\n");
  terminal_printf("  - Command-line shell\n\n");
}

static void cmd_keymap(char *args) {
  // Trim leading spaces
  while (*args == ' ')
    args++;

  if (*args == '\0') {
    // No arguments - show current layout
    terminal_printf("Current keyboard layout: {0E}%s{0F}\n", keyboard_get_layout());
    terminal_printf("Available layouts: us, de\n");
  } else {
    // Set new layout
    keyboard_set_layout(args);
  }
}

// Parse and execute command
void shell_execute(char *cmd) {
  // Trim leading spaces
  while (*cmd == ' ')
    cmd++;

  // Empty command
  if (*cmd == '\0') {
    return;
  }

  // Find first space (separates command from arguments)
  char *args = cmd;
  while (*args && *args != ' ')
    args++;

  size_t cmd_len = args - cmd;

  // Skip spaces to get to arguments
  while (*args == ' ')
    args++;

  // Check commands
  if (strncmp(cmd, "help", cmd_len) == 0 && cmd_len == 4) {
    cmd_help();
  } else if (strncmp(cmd, "clear", cmd_len) == 0 && cmd_len == 5) {
    cmd_clear();
  } else if (strncmp(cmd, "echo", cmd_len) == 0 && cmd_len == 4) {
    cmd_echo(args);
  } else if (strncmp(cmd, "keymap", cmd_len) == 0 && cmd_len == 6) {
    cmd_keymap(args);
  } else if (strncmp(cmd, "about", cmd_len) == 0 && cmd_len == 5) {
    cmd_about();
  } else {
    terminal_printf("{0C}Unknown command: {0F}");
    // Print just the command part
    for (size_t i = 0; i < cmd_len && cmd[i]; i++) {
      terminal_putchar(cmd[i]);
    }
    terminal_printf("\n{0E}Type 'help' for available commands{0F}\n");
  }
}

// Process input character
void shell_input(char c) {
  if (c == '\n') {
    // Execute command
    terminal_putchar('\n');
    command_buffer[buffer_pos] = '\0';
    shell_execute(command_buffer);

    // Reset buffer
    buffer_pos = 0;
    terminal_printf("{0A}> {0F}");
  } else if (c == '\b') {
    // Backspace
    if (buffer_pos > 0) {
      buffer_pos--;
      // Move cursor back, print space, move back again
      terminal_putchar('\b');
      terminal_putchar(' ');
      terminal_putchar('\b');
    }
  } else if ((c >= 32 && c < 127) || (unsigned char)c >= 128) {
    // Printable character (ASCII 32-126 or extended ASCII 128-255)
    if (buffer_pos < SHELL_BUFFER_SIZE - 1) {
      command_buffer[buffer_pos++] = c;
      terminal_putchar(c);
    }
  }
}

// Initialize shell
void shell_init(void) {
  buffer_pos = 0;
  terminal_printf("\n{0B}Welcome to OKOS Shell!{0F}\n");
  terminal_printf("{0E}Type 'help' for available commands{0F}\n\n");
  terminal_printf("{0A}> {0F}");
}
