ifeq ($(MAKECMDGOALS), shi)

include mk/gcc-arm.mk

INC_DIRS += ./inc
INC_DIRS += ./src/lib/core

SRC_DIRS = ./src/lib/api

endif	

shi: $(LIB)
	@echo "Build took $$(($$(date +%s)-$(DATE))) seconds"