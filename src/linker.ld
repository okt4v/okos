ENTRY(_start)

SECTIONS
{
  . = 1M;

  .multiboot : ALIGN(8)
  {
    KEEP(*(.multiboot))
  }

  .note : ALIGN(8)
  {
    *(.note*)
  }

  .text : ALIGN(4K)
  {
    *(.text)
  }

  .rodata : ALIGN(4K)
  {
    *(.rodata)
  }

  .data : ALIGN(4K)
  {
    *(.data)
  }

  .bss : ALIGN(4K)
  {
    *(COMMON)
    *(.bss)
  }
  
  /DISCARD/ :
  {
    *(.eh_frame)
    *(.comment)
  }
}
