CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
BIN = arm-none-eabi-objcopy
STL = st-flash

TARGET = main.bin
LIB_SRC_DIR =./drivers/stm_lib/src
SRC_DIR =./src
ASM_DIR = ./drivers/cmsis_boot/startup

INCLUDE = -Idrivers/cmsis/
INCLUDE += -Idrivers/cmsis_boot/
INCLUDE += -Idrivers/stm_lib/inc
INCLUDE += -Iinc/
LDSCRIPT = STM32F103RBTX_FLASH.ld

CFLAGS = -g -O2 -Wall
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m3 -mthumb-interwork
CFLAGS += -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -DUSE_FULL_ASSERT
CFLAGS += -T$(LDSCRIPT)
CFLAGS += -Wl,--gc-sections 
CFLAGS += $(INCLUDE)
# OBJS = *.o *.elf *.bin

SRC_FILES = $(wildcard $(LIB_SRC_DIR)/*.c)
SRC_FILES += $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(ASM_DIR)/*.s)

C_OBJ = $(SRC_FILES:.c=.o)
ASM_OBJ = $(ASM_FILES:.s=.o)

all: $(TARGET)

print:
	@echo "$(ASM_FILES)"
	@echo "$(ASM_OBJ)"

$(C_OBJ): %.o:%.c
	@echo "[CC] $@, $<"
	$(CC) $(CFLAGS) -c $< -o $@

$(ASM_OBJ): $(ASM_FILES)
	@echo "[AS] $<"
	$(AS) -o $(ASM_OBJ) $(ASM_FILES)

# main.o: main.c
# $(CC) $(CFLAGS) -c -o main.o main.c

main.elf: $(ASM_OBJ) $(C_OBJ) 
	@echo "[LD] $@, $<"
	$(CC) $(CFLAGS) -o main.elf $(ASM_OBJ) $(C_OBJ) 

main.bin: main.elf
	@echo "[BIN] $@, $<"
	$(BIN) -O binary main.elf main.bin

flash: main.bin
	$(STL) --connect-under-reset write main.bin 0x8000000

erase:
	$(STL) erase

.PHONY: clean all
clean:
	rm -f $(C_OBJ) $(ASM_OBJ) $(TARGET)
 
-include $(DEPS)
