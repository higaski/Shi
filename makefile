SHELL = /bin/sh
MKDIR_P = mkdir -p
RM = -rm -rf
CP = cp
DATE := $(shell date +%s)

BUILD_DIR = ./build
TARGET_NAME = $(MAKECMDGOALS)
TARGET_DIR = $(BUILD_DIR)/$(TARGET_NAME)
EXE = $(TARGET_DIR)/$(TARGET_NAME)
HEX = $(TARGET_DIR)/$(TARGET_NAME).hex
ELF = $(TARGET_DIR)/$(TARGET_NAME).elf
LIB = $(TARGET_DIR)/$(TARGET_NAME).a
LST = $(TARGET_DIR)/$(TARGET_NAME).lst
MAP = $(TARGET_DIR)/$(TARGET_NAME).map

all:
	$(MAKE) bench
	$(MAKE) shi
	$(MAKE) test
	
.PHONY: all clean

clean:
	$(RM) $(BUILD_DIR)/*

# Flags
include mk/debugging.mk
include mk/optimizations.mk
include mk/preprocessor.mk
include mk/verbose.mk
include mk/warnings.mk

# Targets
include mk/bench.mk
include mk/quick_n_dirty.mk
include mk/shi.mk
include mk/test.mk

# Generic build rules
include mk/build.mk

# QEMU doesn't null flash memory, so fill with the linker script
ifeq ($(FILL_UNUSED_FLASH),1)
	FILL = -DFILL_UNUSED_FLASH
else
	FILL = -DDONT_FILL_UNUSED_FLASH
endif	

ldscript:
	$(MKDIR_P) $(TARGET_DIR)
	$(CPP) $(FILL) -P ./src/ports/stm32f4discovery/STM32F407VGTx_FLASH.ld -o $(TARGET_DIR)/STM32F407VGTx_FLASH.ld
