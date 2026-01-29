# OKOS Development Session State

## Last Session Date
2026-01-29

## Current Status: German Keyboard Layout Issues

### What's Working ✅
- Basic OS boot and 64-bit long mode
- VGA terminal with color support and printf
- Complete IDT (48 interrupt handlers: 32 CPU exceptions + 16 IRQs)
- PIC initialization and IRQ remapping (32-47)
- PS/2 keyboard driver with US layout
- CLI shell with commands: help, clear, echo, about, keymap
- Keyboard layout switching (US/DE)
- AltGr key detection working
- `@` character works with AltGr+Q

### Current Problem 🐛
**German special characters NOT displaying:**
- ö, ä, ü (umlauts) - NOT working
- Ö, Ä, Ü (uppercase) - NOT working
- ß (sharp s) - NOT working
- < > | keys - NOT working
- Only `@` (AltGr+Q) works

**Root Cause Analysis Needed:**
The shell was updated to allow extended ASCII (128-255) with:
```c
} else if ((c >= 32 && c < 127) || (unsigned char)c >= 128) {
```
This fixed `@` but NOT the umlauts. Likely issues:
1. VGA font might not have correct glyphs for CP437/Latin1 codes
2. Character codes might be wrong for VGA text mode
3. Terminal putchar might need unsigned char handling
4. Scancode table values might be incorrect

**Next Steps:**
1. Check what VGA code page is being used (likely CP437, not Latin1)
2. Replace German character codes with CP437 equivalents:
   - ß: 0xE1 (CP437) instead of 0xDF (Latin1)
   - ä: 0x84 (CP437) instead of 0xE4 (Latin1)
   - ö: 0x94 (CP437) instead of 0xF6 (Latin1)
   - ü: 0x81 (CP437) instead of 0xFC (Latin1)
   - Ä: 0x8E, Ö: 0x99, Ü: 0x9A
3. Test < > | keys (scancodes 86 for German layout)
4. Verify terminal_putchar handles unsigned char properly

## Project Structure
```
okos/
├── src/
│   ├── asm/
│   │   ├── boot.asm          # 64-bit boot, GDT, SS initialization (FIXED!)
│   │   └── interrupt.asm     # ISR/IRQ stubs, iretq handling
│   ├── c/
│   │   ├── kernel/
│   │   │   ├── kernel.c      # Main entry (just calls system_initialize)
│   │   │   ├── initialize.c  # All boot-time initialization
│   │   │   └── shell.c       # CLI with command parsing
│   │   ├── drivers/
│   │   │   ├── terminal.c    # VGA text mode (80x25)
│   │   │   ├── keyboard.c    # PS/2 driver, US/DE layouts, AltGr
│   │   │   └── pic.c         # 8259 PIC driver
│   │   └── cpu/
│   │       └── idt.c         # IDT setup, exception/IRQ handlers
│   └── inc/                  # Headers
├── build/                    # Compiled objects
├── iso/                      # ISO filesystem
├── Makefile                  # Recursive build system
└── okos.iso                  # Bootable image
```

## Critical Fixes Applied

### 1. IDT Structure Bug (MAJOR FIX)
**File:** `src/c/cpu/idt.c`
**Issue:** `offset_high` was `uint16_t` but needs `uint32_t` in 64-bit mode
```c
struct idt_entry {
  uint16_t offset_low;
  uint16_t selector;
  uint8_t ist;
  uint8_t type_attr;
  uint16_t offset_mid;
  uint32_t offset_high;  // FIXED: was uint16_t
  uint32_t zero;
} __attribute__((packed));
```

### 2. SS Register Bug (CRITICAL FIX)
**File:** `src/asm/boot.asm`
**Issue:** SS was 0x18 (invalid GDT selector), causing GPF on iretq
**Fix:** Use iretq trick to set SS=0x10 properly in 64-bit mode
```asm
bits 64
long_mode_start:
    mov rsp, 0x200000
    and rsp, -16

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Set SS using iretq
    push 0x10          ; SS
    push rsp           ; RSP
    pushfq             ; RFLAGS
    push 0x08          ; CS
    lea rax, [rel .reload_segments]
    push rax
    iretq
.reload_segments:
    ; SS now = 0x10
```

### 3. GDT Data Segment Fix
**File:** `src/asm/boot.asm`
**Issue:** Data segment missing writable bit (bit 41)
```asm
gdt64:
    dq 0                                    ; Null
.code: equ 0x08
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53)  ; Code: exec, present, 64-bit
.data: equ 0x10
    dq (1<<41) | (1<<44) | (1<<47)         ; Data: writable, present (FIXED)
```

## Keyboard Layouts Implemented

### US QWERTY (Working ✅)
- Standard US layout
- Shift support
- Caps Lock support

### German (de-latin1) - PARTIALLY WORKING ⚠️
**Working:**
- Basic letters (a-z, A-Z)
- Numbers and symbols
- AltGr+Q = @ ✅

**NOT Working:**
- Umlauts: ö ä ü Ö Ä Ü ❌
- Special: ß < > | ❌

**Current Character Codes (Latin1 - WRONG for VGA!):**
```c
// These use Latin1 encoding, but VGA uses CP437!
de_scancode_to_ascii[12] = 0xDF;  // ß
de_scancode_to_ascii[26] = 0xFC;  // ü
de_scancode_to_ascii[39] = 0xF6;  // ö
de_scancode_to_ascii[40] = 0xE4;  // ä
```

**Should be CP437:**
```c
// NEED TO CHANGE TO THESE:
ß: 0xE1
ü: 0x81 (lowercase)
ö: 0x94 (lowercase)
ä: 0x84 (lowercase)
Ü: 0x9A (uppercase)
Ö: 0x99 (uppercase)
Ä: 0x8E (uppercase)
```

## Shell Commands
- `help` - Show all commands
- `clear` - Clear screen
- `echo TEXT` - Print text
- `keymap` - Show current layout
- `keymap us` - Switch to US
- `keymap de` - Switch to German
- `about` - OS info

## Build Commands
```bash
make clean       # Clean build artifacts
make            # Build kernel.bin
make iso        # Create bootable ISO
make run        # Run in QEMU
make verify     # Verify multiboot2
```

## Technical Details

### Memory Map
- Kernel loads at: 0x100000 (1MB)
- Stack at: 0x200000 (2MB, 16KB)
- VGA buffer: 0xB8000
- Identity mapped: First 1GB (2MB pages)

### GDT Selectors
- 0x00: Null descriptor
- 0x08: Code segment (64-bit)
- 0x10: Data segment (writable, used for SS)

### Interrupt Vectors
- 0-31: CPU exceptions
- 32-47: Hardware IRQs (PIC remapped)
  - IRQ0 (32): Timer (not enabled)
  - IRQ1 (33): Keyboard ✅
  - IRQ2-15: Other devices (masked)

### Important Scancodes
- 0x2A, 0x36: Left/Right Shift
- 0x38: Left Alt (ignored)
- E0 0x38: Right Alt (AltGr) ✅
- 0x3A: Caps Lock
- 0x56 (86): < key on German keyboard

## Known Issues to Fix Next Session

1. **PRIORITY: German character display**
   - Replace Latin1 codes with CP437 codes
   - Test terminal_putchar with unsigned values
   - Verify scancode table entries

2. **< > | keys not working**
   - Scancode 86 (0x56) placement
   - Check if it's being filtered somewhere

3. **Shell improvements needed:**
   - Command history (up/down arrow)
   - Better backspace (currently basic)
   - Tab completion

4. **Future features:**
   - Timer/RTC for uptime command
   - Memory info command
   - More keyboard layouts

## Debug Notes
- VGA text mode uses Code Page 437, NOT Latin1!
- `char` is signed (-128 to 127), use `unsigned char` for values > 127
- E0 prefix required for extended keys (AltGr = E0 38)
- `iretq` pops: RIP, CS, RFLAGS, RSP, SS (all 64-bit)

## Contact Points for Continuation
- Last working: Basic shell with US keyboard
- Next task: Fix German umlaut display (CP437 codes)
- After that: Implement < > | keys properly
- User wants fully working German keyboard layout

## File Locations for Quick Access
- Keyboard driver: `src/c/drivers/keyboard.c`
- Shell: `src/c/kernel/shell.c`
- Terminal: `src/c/drivers/terminal.c`
- Boot/GDT: `src/asm/boot.asm`
- IDT: `src/c/cpu/idt.c`
