ASM = nasm
ASMFLAGS = -f elf64 

CC = gcc
CFLAGS = -freestanding -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -Wall -Wextra

LD = ld 
LDFLAGS = -n -T linker.ld 

SRC = src
ASMSRC = src/asm
CSRC = src/c 

BUILD = build
ISO = iso

all:
	@echo "[ …… ] Dir check"
	@mkdir -p build iso/boot/grub
	@echo "[ ✔  ] Dir check"


boot.o: $(ASMSRC)/boot.asm
	@$(ASM) $(ASMFLAGS) $(ASMSRC)/boot.asm -o $(BUILD)/boot.o

kernel.o: $(CSRC)/kernel.c 
	@$(CC) $(CFLAGS) -c $(CSRC)/kernel.c -o $(BUILD)/kernel.o 

kernel.bin: $(BUILD)/boot.o $(BUILD)/kernel.o 
	$(LD) $(LDFLAGS) -o $(BUILD)/kernel.bin $(BUILD)/boot.o $(BUILD)/kernel.o 

iso:
	@cp $(BUILD)/kernel.bin $(ISO)/boot/
	@cp $(SRC)/grub.cfg $(ISO)/boot/grub/
	grub-mkrescue -o okos.iso $(ISO)

verify:
	grub-file --is-x86-multiboot2 $(BUILD)/kernel.bin $$ echo "[ ✔  ] Valid Multiboot2 kernel" || echo "[ ✖  ] Invalid Multiboot2 kernel"

clean:
	@rm -r $(BUILD) 

.PHONY: all clean verify























