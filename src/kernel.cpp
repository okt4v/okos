extern "C" {
void kernel_main();
}

void kernel_main() {
  volatile char *video_memory = (volatile char *)0xb8000;
  video_memory[0] = 'A';
  video_memory[1] = 0x0F;

  while (1) {
    __asm__("hlt");
  }
}
