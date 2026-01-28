#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>

#define SHELL_BUFFER_SIZE 256
#define SHELL_MAX_ARGS 16

// Initialize the shell
void shell_init(void);

// Process a character from keyboard
void shell_input(char c);

// Execute a command
void shell_execute(char *cmd);

#endif
