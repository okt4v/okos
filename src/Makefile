# Variables
CC = x86_64-elf-g++ #compiler
LD = x86_64-elf-ld  #linker
CFLAGS = -ffreestanding -c -m64 
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


