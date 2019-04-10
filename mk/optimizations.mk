# Clang has an -Oz flag which is basically the same as GCCs -Os
ifeq ($(CXX),$(filter $(CXX),g++ arm-none-eabi-g++))
ifeq ($(OPTIMIZE),1)
	OPTFLAGS += -Os
	OPTFLAGS += -finline-small-functions
else
	OPTFLAGS += -O0
endif
else
ifeq ($(OPTIMIZE),1)
	OPTFLAGS += -Oz
else	
	OPTFLAGS += -O0
endif
endif

ifeq ($(LTO),1)
	OPTFLAGS+= -flto
	OPTFLAGS+= -ffat-lto-objects
endif

OPTFLAGS += -ffunction-sections
OPTFLAGS += -fdata-sections
OPTFLAGS += -fomit-frame-pointer