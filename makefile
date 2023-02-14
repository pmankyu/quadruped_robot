CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-gcc
BIN = arm-none-eabi-objcopy
STL = st-flash

TARGET = main
LIB_SRC_DIR =./drivers/stm_lib/src
SRC_DIR =./src
ASM_DIR = ./drivers/cmsis_boot/startup
BIN_DIR = ./bin

INCLUDE = -Idrivers/cmsis/
INCLUDE += -Idrivers/cmsis_boot/
INCLUDE += -Idrivers/stm_lib/inc
INCLUDE += -Iinc/

LDSCRIPT = STM32F103RBTX_FLASH.ld

CFLAGS = -g -O2 -Wall
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m3 -mthumb-interwork
CFLAGS += -DSTM32F10X_MD
CFLAGS += -DUSE_STDPERIPH_DRIVER
CFLAGS += -DUSE_FULL_ASSERT
CFLAGS += $(INCLUDE)

LDFLAGS = -T$(LDSCRIPT)
LDFLAGS += -mthumb
LDFLAGS += -mcpu=cortex-m3
#LDFLAGS += --specs=nosys.specs
LDFLAGS += --specs=nano.specs
LDFLAGS += -lc
LDFLAGS += -Wl,--gc-sections 
LDFLAGS += -u _printf_float

SRC_FILES = $(wildcard $(LIB_SRC_DIR)/*.c)
SRC_FILES += $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(ASM_DIR)/*.s)

C_OBJ = $(SRC_FILES:.c=.o)
ASM_OBJ = $(ASM_FILES:.s=.o)

all: $(BIN_DIR)/$(TARGET).bin

$(C_OBJ): %.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(ASM_OBJ): $(ASM_FILES)
	$(AS) -o $(ASM_OBJ) $(ASM_FILES)

$(BIN_DIR)/$(TARGET).elf: $(ASM_OBJ) $(C_OBJ) 
	$(LD) $(LDFLAGS) -o $(BIN_DIR)/$(TARGET).elf $(ASM_OBJ) $(C_OBJ) 

$(BIN_DIR)/$(TARGET).bin: $(BIN_DIR)/$(TARGET).elf
	$(BIN) -O binary $(BIN_DIR)/$(TARGET).elf $(BIN_DIR)/$(TARGET).bin

flash: $(BIN_DIR)/$(TARGET).bin
	$(STL) --connect-under-reset write $(BIN_DIR)/$(TARGET).bin 0x8000000

erase:
	$(STL) erase

.PHONY: clean all
clean:
	rm -f $(C_OBJ) $(ASM_OBJ)
	rm -f $(BIN_DIR)/*
 
-include $(DEPS)
