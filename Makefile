CC = gcc
LD = ld
AS = nasm

CFLAGS = -m32 -ffreestanding -fno-pic -fno-pie -Wall -Wextra
LDFLAGS = -m elf_i386 -T linker.ld

all: liteos.iso

liteos.iso: kernel.elf grub/grub.cfg
	mkdir -p iso/boot/grub
	cp kernel.elf iso/boot/
	cp grub/grub.cfg iso/boot/grub/
	grub-mkrescue -o $@ iso/

kernel.elf: boot.o kernel.o
	$(LD) $(LDFLAGS) -o $@ $^

boot.o: boot.asm
	$(AS) -f elf32 $< -o $@
	
kernel.o: kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.bin *.iso *.elf
	rm -rf iso/

rebuild: clean all

run: liteos.iso
	qemu-system-x86_64 -cdrom liteos.iso -serial stdio

run-release: liteos.iso
	qemu-system-x86_64 -cdrom liteos.iso