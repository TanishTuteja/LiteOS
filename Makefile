CC = gcc
CFLAGS = -ffreestanding -Wall -Wextra

all: kernel.o

kernel.o: kernel.c
	$(CC) $(CFLAGS) -c kernel.c -o kernel.o

clean:
	rm -f kernel.o

rebuild: clean all