CC = clang
LD = ld.lld
CFLAGS = -target arm-none-eabi -mcpu=cortex-m3 -ffreestanding -mthumb -Iinclude -Iinclude/lwip
SRC = $(shell find src -name "*.c" | grep -v "ppp" | grep -v "slipif")
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
	find src -name "*.o" -delete
	rm -f boot/*.o kernel.elf