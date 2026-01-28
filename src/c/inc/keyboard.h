#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

// Keyboard data and command ports
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// Initialize keyboard driver
void keyboard_install(void);

// Keyboard interrupt handler
void keyboard_handler(void);

// Set keyboard layout (e.g., "us", "de")
void keyboard_set_layout(const char *layout_name);

// Get current keyboard layout
const char* keyboard_get_layout(void);

#endif
