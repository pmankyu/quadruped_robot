CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
BIN = arm-none-eabi-objcopy
STL = st-flash
CMSIS_DEV_PATH = ../files/STM32F10x_StdPeriph_Lib_V3.6.0/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x
CMSIS_CORE_PATH = ../files/STM32F10x_StdPeriph_Lib_V3.6.0/Libraries/CMSIS/CM3/CoreSupport
INC_PATH =../files/STM32F10x_StdPeriph_Lib_V3.6.0/Libraries/STM32F10x_StdPeriph_Driver/inc
SRC_PATH =../files/STM32F10x_StdPeriph_Lib_V3.6.0/Libraries/STM32F10x_StdPeriph_Driver/src
CFLAGS = -mthumb -mcpu=cortex-m3 -I$(CMSIS_DEV_PATH) -I$(CMSIS_CORE_PATH) -I$(INC_PATH) -I$(SRC_PATH)

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
