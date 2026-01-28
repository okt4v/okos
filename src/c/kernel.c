void kernel_main(unsigned long magic, unsigned long addr) {
  while (1) {
    __asm__("hlt");
  }
}
