CC = gcc
LD = ld

CFLAGS = -ffreestanding -Wall -Wextra
LDFLAGS = -T linker.ld --oformat binary

all: kernel.bin

kernel.bin: kernel.o
	$(LD) $(LDFLAGS) -o $@ $^

kernel.o: kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.bin

rebuild: clean all