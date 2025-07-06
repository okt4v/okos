OUTPUT_FORMAT("elf64-x86_64")
ENTRY(_start)
SECTIONS
{
  . = 0x100000; /* Start address for the kernel */

  .text : {
    *(.text)
  }

  .bss : {
    *(.bss)
    *(COMMON)
  }
}
