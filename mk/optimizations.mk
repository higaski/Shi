# Clang has an -Oz flag which is basically the same as GCCs -Os
ifeq ($(CXX),$(filter $(CXX),g++ arm-none-eabi-g++))
ifdef OPTIMIZE
	OPTFLAGS += -Os
	OPTFLAGS += -finline-small-functions
else
	OPTFLAGS += -O0
endif
else
ifdef OPTIMIZE
	OPTFLAGS += -Oz
else	
	OPTFLAGS += -O0	
endif
endif

ifdef LTO
	OPTFLAGS+= -flto
endif

OPTFLAGS += -ffunction-sections
OPTFLAGS += -fdata-sections
OPTFLAGS += -fomit-frame-pointer