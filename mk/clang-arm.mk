AS = clang
CC = clang
CXX = clang++
LD = clang++
AR = llvm-ar
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE = arm-none-eabi-size

# Manually add arm-none-eabi include folders
INC_DIRS += /usr/arm-none-eabi/include
INC_DIRS += /usr/arm-none-eabi/include/c++/8.2.0
INC_DIRS += /usr/arm-none-eabi/include/c++/8.2.0/arm-none-eabi/thumb/v7e-m+fp/hard/
INC_DIRS += /usr/arm-none-eabi/include/c++/8.2.0/backward
INC_DIRS += /usr/lib/gcc/arm-none-eabi/8.2.0/include
INC_DIRS += /usr/lib/gcc/arm-none-eabi/8.2.0/include-fixed

# Create include flags
INC_FLAGS = $(addprefix -I,$(INC_DIRS))

# Preprocessor flags
CPPFLAGS += $(INC_FLAGS) -MMD -MP

# Type definitions taken from arm-none-eabi-g++ predefines
CPPFLAGS += -D__CHAR16_TYPE__="short unsigned int"
CPPFLAGS += -D__CHAR32_TYPE__="long unsigned int"
CPPFLAGS += -D__INT16_TYPE__="short int"
CPPFLAGS += -D__INT32_TYPE__="long int"
CPPFLAGS += -D__INT64_TYPE__="long long int"
CPPFLAGS += -D__INT8_TYPE__="signed char"
CPPFLAGS += -D__INTMAX_TYPE__="long long int"
CPPFLAGS += -D__INTPTR_TYPE__="int"
CPPFLAGS += -D__INT_FAST16_TYPE__="int"
CPPFLAGS += -D__INT_FAST32_TYPE__="int"
CPPFLAGS += -D__INT_FAST64_TYPE__="long long int"
CPPFLAGS += -D__INT_FAST8_TYPE__="int"
CPPFLAGS += -D__INT_LEAST16_TYPE__="short int"
CPPFLAGS += -D__INT_LEAST32_TYPE__="long int"
CPPFLAGS += -D__INT_LEAST64_TYPE__="long long int"
CPPFLAGS += -D__INT_LEAST8_TYPE__="signed char"
CPPFLAGS += -D__PTRDIFF_TYPE__="int"
CPPFLAGS += -D__SIG_ATOMIC_TYPE__="int"
CPPFLAGS += -D__SIZE_TYPE__="unsigned int"
CPPFLAGS += -D__UINT16_TYPE__="short unsigned int"
CPPFLAGS += -D__UINT32_TYPE__="long unsigned int"
CPPFLAGS += -D__UINT64_TYPE__="long long unsigned int"
CPPFLAGS += -D__UINT8_TYPE__="unsigned char"
CPPFLAGS += -D__UINTMAX_TYPE__="long long unsigned int"
CPPFLAGS += -D__UINTPTR_TYPE__="unsigned int"
CPPFLAGS += -D__UINT_FAST16_TYPE__="unsigned int"
CPPFLAGS += -D__UINT_FAST32_TYPE__="unsigned int"
CPPFLAGS += -D__UINT_FAST64_TYPE__="long long unsigned int"
CPPFLAGS += -D__UINT_FAST8_TYPE__="unsigned int"
CPPFLAGS += -D__UINT_LEAST16_TYPE__="short unsigned int"
CPPFLAGS += -D__UINT_LEAST32_TYPE__="long unsigned int"
CPPFLAGS += -D__UINT_LEAST64_TYPE__="long long unsigned int"
CPPFLAGS += -D__UINT_LEAST8_TYPE__="unsigned char"
CPPFLAGS += -D__WCHAR_TYPE__="unsigned int"
CPPFLAGS += -D__WINT_TYPE__="unsigned int"

# Common flags
FLAGS += $(VFLAG)
FLAGS += --target=arm-none-eabi -march=armv7e-m -mcpu=cortex-m4 -mthumb -mlittle-endian -mfloat-abi=hard -mfpu=fpv4-sp-d16
FLAGS += -fmessage-length=0 -fsigned-char
FLAGS += --sysroot=/usr/arm-none-eabi/
FLAGS += $(OPTFLAGS)
FLAGS += $(WARNFLAGS)
FLAGS += $(DBGFLAGS)
FLAGS += -fuse-ld=lld #gold/bfd

# Assembler flags
ASFLAGS += $(FLAGS)
ASFLAGS += -x assembler-with-cpp

# C flags
CFLAGS += $(FLAGS)
CFLAGS += -std=gnu11

# C++ flags
CXXFLAGS += $(FLAGS)
CXXFLAGS += -std=c++2a -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics -ftemplate-depth=2048 -ftemplate-backtrace-limit=0
 
# Linker flags
LDFLAGS += $(FLAGS)
LDFLAGS += -nostartfiles
LDFLAGS += -nostdlib
LDFLAGS += -Wl,--gc-sections,-Map=$(MAP) 
LDFLAGS += /usr/arm-none-eabi/lib/crt0.o
LDFLAGS += /usr/lib/gcc/arm-none-eabi/8.2.0/thumb/v7e-m+fp/hard/crti.o
LDFLAGS += /usr/lib/gcc/arm-none-eabi/8.2.0/thumb/v7e-m+fp/hard/crtbegin.o
LDFLAGS += /usr/lib/gcc/arm-none-eabi/8.2.0/thumb/v7e-m+fp/hard/crtn.o
LDFLAGS += /usr/lib/gcc/arm-none-eabi/8.2.0/thumb/v7e-m+fp/hard/crtend.o
LDFLAGS += -L"/usr/arm-none-eabi/lib/thumb/v7e-m+fp/hard/"
LDFLAGS += -L"/usr/lib/gcc/arm-none-eabi/8.2.0/thumb/v7e-m+fp/hard/"
LDLIBS += -lstdc++_nano -lm -lgcc -lc_nano