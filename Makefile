ASM = nasm
ASMFLAGS = -f elf64 
CC = gcc
CFLAGS = -ffreestanding -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -Wall -Wextra -I$(CSRC)
LD = ld 
LDFLAGS = -n -T

SRC = src
ASMSRC = src/asm
CSRC = src/c
BUILD = build
ISO = iso

# Automatically find all source files (including subdirectories)
ASM_SOURCES = $(shell find $(ASMSRC) -name '*.asm' 2>/dev/null)
C_SOURCES = $(shell find $(CSRC) -name '*.c' 2>/dev/null)

# Generate object file names (flatten to just filename, no paths)
ASM_OBJS = $(addprefix $(BUILD)/,$(notdir $(ASM_SOURCES:.asm=.o)))
C_OBJS = $(addprefix $(BUILD)/,$(notdir $(C_SOURCES:.c=.o)))

# All object files
OBJS = $(ASM_OBJS) $(C_OBJS)

all: $(BUILD)/kernel.bin
	@echo "[ ✔  ] Build complete"

# Create build directory
$(BUILD):
	@echo "[ …… ] Creating build directory"
	@mkdir -p $(BUILD)
	@echo "[ ✔  ] Build directory created"

# Compile each .o file by finding its corresponding source
$(BUILD)/%.o: | $(BUILD)
	@echo "[ …… ] Building $@"
	@SRC_FILE=$$(find $(ASMSRC) -name '$*.asm' 2>/dev/null | head -1); \
	if [ -n "$$SRC_FILE" ]; then \
		echo "[ …… ] Assembling $$SRC_FILE"; \
		$(ASM) $(ASMFLAGS) $$SRC_FILE -o $@; \
		echo "[ ✔  ] $@ created"; \
	else \
		SRC_FILE=$$(find $(CSRC) -name '$*.c' 2>/dev/null | head -1); \
		if [ -n "$$SRC_FILE" ]; then \
			echo "[ …… ] Compiling $$SRC_FILE"; \
			$(CC) $(CFLAGS) -c $$SRC_FILE -o $@; \
			echo "[ ✔  ] $@ created"; \
		else \
			echo "[ ✖  ] Source file for $@ not found"; \
			exit 1; \
		fi; \
	fi

# Link all object files
$(BUILD)/kernel.bin: $(OBJS)
	@echo "[ …… ] Linking kernel"
	@$(LD) $(LDFLAGS) $(SRC)/linker.ld -o $(BUILD)/kernel.bin $(OBJS)
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
	@qemu-system-x86_64 -kernel $(BUILD)/kernel.bin || echo "Direct boot not available, please use 'make run'"

run: iso
	@echo "[ …… ] Running ISO in QEMU"
	@qemu-system-x86_64 -cdrom okos.iso

clean:
	@echo "[ …… ] Cleaning build artifacts"
	@rm -rf $(BUILD) $(ISO) okos.iso
	@echo "[ ✔  ] Clean complete"

# Debug target to see what files were found
debug:
	@echo "ASM sources: $(ASM_SOURCES)"
	@echo "C sources: $(C_SOURCES)"
	@echo "ASM objects: $(ASM_OBJS)"
	@echo "C objects: $(C_OBJS)"
	@echo "All objects: $(OBJS)"

.PHONY: all iso verify run run-fast clean debug
