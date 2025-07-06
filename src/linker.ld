OUTPUT_FORMAT("elf64-x86-64")
ENTRY(start)
SECTIONS {
    . = 1M;
    .text : { *(.text) }
    .data : { *(.data) }
    .bss : { *(.bss) }
}
