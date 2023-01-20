CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
BIN = arm-none-eabi-objcopy
STL = st-flash
CFLAGS = -mthumb -mcpu=cortex-m3

all: main.bin

crt.o: crt.s
	$(AS) -o crt.o crt.s

main.o: main.c
	$(CC) $(CFLAGS) -c -o main.o main.c

main.elf: linker.ld crt.o main.o
	$(LD) -T linker.ld -o main.elf crt.o main.o

main.bin: main.elf
	$(BIN) -O binary main.elf main.bin

clean:
	rm -f *.o *.elf *.bin

flash: main.bin
	$(STL) --connect-under-reset write main.bin 0x8000000

erase:
	$(STL) erase
