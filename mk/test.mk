ifeq ($(MAKECMDGOALS), test)

include mk/gcc-arm.mk

CPPFLAGS += -DSTM32F407xx
#CPPFLAGS += -D__FPU_PRESENT
CPPFLAGS += -DARM_MATH_CM4
CPPFLAGS += -DDEBUG
CPPFLAGS += -DOS_USE_SEMIHOSTING
CPPFLAGS += -DTRACE
CPPFLAGS += -DOS_USE_TRACE_SEMIHOSTING_STDOUT

INC_DIRS += ./inc
INC_DIRS += ./src/lib/core

SRC_DIRS = ./src/test
SRC_DIRS += ./src/lib/api

LDFLAGS += -T"./src/test/ldscripts/mem.ld"
LDFLAGS += -T"./src/test/ldscripts/sections.ld"

endif	

test: $(HEX)
	@echo "Build took $$(($$(date +%s)-$(DATE))) seconds"