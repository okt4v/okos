void kernel_main(unsigned long magic, unsigned long addr) {
  (void)magic;
  (void)addr;

  // Write "OK" to screen
  unsigned char *vga = (unsigned char *)0xB8000;
  vga[0] = 'O';
  vga[1] = 0x0F;
  vga[2] = 'K';
  vga[3] = 0x0F;

  while (1) {
    __asm__("hlt");
  }
}
