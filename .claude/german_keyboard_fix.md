# German Keyboard Fix - CP437 Character Codes

## Problem
VGA text mode uses **Code Page 437 (CP437)**, not Latin-1!
The German characters are using wrong codes.

## Fix Required in `src/c/drivers/keyboard.c`

Replace these character codes in the German scancode tables:

### Lowercase (de_scancode_to_ascii[])
```c
// CHANGE THESE:
0xDF  →  0xE1   // ß (sharp s)
0xFC  →  0x81   // ü (u-umlaut)
0xF6  →  0x94   // ö (o-umlaut)
0xE4  →  0x84   // ä (a-umlaut)
```

### Uppercase (de_scancode_to_ascii_shift[])
```c
// CHANGE THESE:
0xA7  →  0x15   // § (section sign)
0xDC  →  0x9A   // Ü (U-umlaut)
0xD6  →  0x99   // Ö (O-umlaut)
0xC4  →  0x8E   // Ä (A-umlaut)
0xB0  →  0xF8   // ° (degree sign)
```

## Quick Reference: CP437 vs Latin-1

| Char | Latin-1 | CP437 | VGA displays |
|------|---------|-------|--------------|
| ß    | 0xDF    | 0xE1  | ß            |
| ä    | 0xE4    | 0x84  | ä            |
| ö    | 0xF6    | 0x94  | ö            |
| ü    | 0xFC    | 0x81  | ü            |
| Ä    | 0xC4    | 0x8E  | Ä            |
| Ö    | 0xD6    | 0x99  | Ö            |
| Ü    | 0xDC    | 0x9A  | Ü            |
| §    | 0xA7    | 0x15  | §            |
| °    | 0xB0    | 0xF8  | °            |

## Also Check
- Scancode 86 (0x56) for < > | keys
- Make sure it's at the right array index
- Current location seems correct at end of array

## After Fix
Rebuild and test:
```bash
make clean && make iso && make run
keymap de
# Then type: äöüßÄÖÜ<>|@
```
