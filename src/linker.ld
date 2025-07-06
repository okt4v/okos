OUTPUT_FORMAT("elf64-x86-64")
ENTRY(_start)
SECTIONS {
    . = 1M;
    .multiboot : { *(.multiboot) }
    .text : { *(.text) *(.rodata*)}
    .data : { *(.data) }
    .bss : { *(.bss) }

    /DISCARD/ : { *(.eh_frame) *(.comment) }
}
