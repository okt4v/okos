# Variables
CC = x86_64-elf-g++#compiler
LD = x86_64-elf-ld#linker
CFLAGS = -ffreestanding -nostdlib -c -m64
LDFLAGS = -T src/linker.ld

# File paths
KERNEL_SRC = src/kernel.cpp
BOOT_SRC = src/boot.asm

# Path vars
BUILD_DIR = build
ISO_DIR = iso
ISO_BOOT_DIR = iso/boot
ISO_BOOT_GRUB_DIR = iso/boot/grub
GRUB_DIR = grub

# Output files
KERNEL_OBJ = $(BUILD_DIR)/kernel.o
BOOT_OBJ = $(BUILD_DIR)/boot.o
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
ISO_IMAGE = $(ISO_DIR)/okos.iso

# Default target
all: $(ISO_IMAGE)

# create iso rules
$(ISO_IMAGE): $(KERNEL_BIN) $(GRUB_DIR)/grub.cfg
	mkdir -p $(ISO_BOOT_GRUB_DIR)
	cp $(KERNEL_BIN) $(ISO_BOOT_GRUB_DIR)/kernel.bin
	grub-mkrescue -o $(ISO_IMAGE) $(ISO_DIR)

# rule to link kernel
$(KERNEL_BIN): $(KERNEL_OBJ) $(BOOT_OBJ)
	$(LD) $(LDFLAGS) -o $(KERNEL_BIN) $(KERNEL_OBJ) $(BOOT_OBJ)

# rule to compite c++ kernel
$(KERNEL_OBJ): $(KERNEL_SRC)
	mkdir -p  $(BUILD_DIR)
	$(CC) $(CFLAGS) $(KERNEL_SRC) -o $(KERNEL_OBJ)

# rule to assemble the bootloader
$(BOOT_OBJ): $(BOOT_SRC)
	mkdir -p $(BUILD_DIR)
	nasm -f elf64 $(BOOT_SRC) -o $(BOOT_OBJ)

# rules to make grub.cfg file
$(GRUB_DIR)/grub.cfg:
	mkdir -p $(ISO_BOOT_GRUB_DIR)
	mkdir -p $(GRUB_DIR)
	echo "set timeout=0" > $(GRUB_DIR)/grub.cfg
	echo "set default=0" >> $(GRUB_DIR)/grub.cfg
	echo 'menuentry "okos" {' >> $(GRUB_DIR)/grub.cfg
	echo "	multiboot /boot/grub/kernel.bin" >> $(GRUB_DIR)/grub.cfg
	echo "	boot" >> $(GRUB_DIR)/grub.cfg
	echo "}" >> $(GRUB_DIR)/grub.cfg
	cp $(GRUB_DIR)/grub.cfg $(ISO_BOOT_GRUB_DIR)

clean:
	rm -rf $(BUILD_DIR) $(ISO_DIR) $(GRUB_DIR)
















