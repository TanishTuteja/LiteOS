CC = gcc
LD = ld
AS = nasm

CFLAGS = -m32 -ffreestanding -fno-pic -fno-pie -Wall -Wextra
LDFLAGS = -m elf_i386 -T linker.ld --oformat binary

all: kernel.bin

kernel.bin: boot.o kernel.o
	$(LD) $(LDFLAGS) -o $@ $^

boot.o: boot.asm
	$(AS) -f elf32 $< -o $@
	
kernel.o: kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.bin

rebuild: clean all