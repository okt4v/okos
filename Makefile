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

all:
	@mkdir -p build



boot.o: $(ASMSRC)/boot.asm
	@$(ASM) $(ASMFLAGS) $(ASMSRC)/boot.asm -o $(BUILD)/boot.o

kernel.o: $(CSRC)/kernel.c 
	@$(CC) $(CFLAGS) -c $(CSRC)/kernel.c -o $(BUILD)/kernel.o 


clean:
	@rm -r $(BUILD)
























