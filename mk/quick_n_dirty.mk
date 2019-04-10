ifeq ($(MAKECMDGOALS), quick_n_dirty)

include mk/gcc-arm.mk

CPPFLAGS += -DSTM32F407xx
#CPPFLAGS += -D__FPU_PRESENT
CPPFLAGS += -DARM_MATH_CM4
CPPFLAGS += -DDEBUG
CPPFLAGS += -DOS_USE_SEMIHOSTING
CPPFLAGS += -DTRACE
CPPFLAGS += -DOS_USE_TRACE_SEMIHOSTING_STDOUT
CPPFLAGS += -DUNITY_SUPPORT_64
CPPFLAGS += -DAPP_MAIN_QUICK_N_DIRTY

INC_DIRS += ./inc
INC_DIRS += ./src/lib/core

SRC_DIRS = ./src/lib/api
SRC_DIRS += ./src/ports/stm32f4discovery
SRC_DIRS += ./src/test

LDFLAGS += -T"$(TARGET_DIR)/STM32F407VGTx_FLASH.ld"

endif	

quick_n_dirty: ldscript $(HEX)
	@echo "Build took $$(($$(date +%s)-$(DATE))) seconds"