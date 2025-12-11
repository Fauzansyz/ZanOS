CC = clang
LD = ld.lld
CFLAGS = -target arm-none-eabi -ffreestanding -mthumb -Iinclude
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
BOOT = boot/start.o

all: kernel.elf

kernel.elf: $(OBJ) $(BOOT) linker.ld
	$(LD) -nostdlib -T linker.ld $(BOOT) $(OBJ) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

boot/start.o: boot/start.S
	$(CC) $(CFLAGS) -c $< -o $@

run:
	qemu-system-arm -M lm3s6965evb -nographic -kernel kernel.elf

clean:
	rm -f src/*.o boot/*.o kernel.elf