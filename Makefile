ASM = nasm
ASMFLAGS = -f elf64 
CC = gcc
CFLAGS = -ffreestanding -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -Wall -Wextra
LD = ld 
LDFLAGS = -n -T
SRC = src
ASMSRC = src/asm
CSRC = src/c
BUILD = build
ISO = iso

all: $(BUILD)/kernel.bin
	@echo "[ ✔  ] Build complete"

$(BUILD)/boot.o: | $(BUILD)
$(BUILD)/kernel.o: | $(BUILD)

$(BUILD):
	@echo "[ …… ] Creating build directory"
	@mkdir -p $(BUILD)
	@echo "[ ✔  ] Build directory created"

$(BUILD)/boot.o: $(ASMSRC)/boot.asm
	@echo "[ …… ] Assembling boot.asm"
	@$(ASM) $(ASMFLAGS) $(ASMSRC)/boot.asm -o $(BUILD)/boot.o
	@echo "[ ✔  ] boot.o created"

$(BUILD)/kernel.o: $(CSRC)/kernel.c 
	@echo "[ …… ] Compiling kernel.c"
	@$(CC) $(CFLAGS) -c $(CSRC)/kernel.c -o $(BUILD)/kernel.o
	@echo "[ ✔  ] kernel.o created"

$(BUILD)/kernel.bin: $(BUILD)/boot.o $(BUILD)/kernel.o 
	@echo "[ …… ] Linking kernel"
	@$(LD) $(LDFLAGS) $(SRC)/linker.ld -o $(BUILD)/kernel.bin $(BUILD)/boot.o $(BUILD)/kernel.o
	@echo "[ ✔  ] kernel.bin created"

iso: $(BUILD)/kernel.bin
	@echo "[ …… ] Creating ISO"
	@mkdir -p $(ISO)/boot/grub
	@cp $(BUILD)/kernel.bin $(ISO)/boot/
	@cp $(SRC)/grub.cfg $(ISO)/boot/grub/
	@grub-mkrescue -o okos.iso $(ISO)
	@echo "[ ✔  ] ISO created"

verify: $(BUILD)/kernel.bin
	@grub-file --is-x86-multiboot2 $(BUILD)/kernel.bin && echo "[ ✔  ] Valid Multiboot2 kernel" || echo "[ ✖  ] Invalid Multiboot2 kernel"

run-fast: $(BUILD)/kernel.bin
	@echo "[ …… ] Running kernel in QEMU"
	@qemu-system-x86_64 -kernel $(BUILD)/kernel.bin || echo "Direct boot no available, please use 'make run'"

run: iso
	@echo "[ …… ] Running ISO in QEMU"
	@qemu-system-x86_64 -cdrom okos.iso

clean:
	@echo "[ …… ] Cleaning build artifacts"
	@rm -rf $(BUILD) $(ISO) okos.iso
	@echo "[ ✔  ] Clean complete"

.PHONY: all iso verify run run-fast clean
