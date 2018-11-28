AS = arm-none-eabi-gcc
CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
LD = arm-none-eabi-g++
AR = arm-none-eabi-ar
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE = arm-none-eabi-size

# Manually add arm-none-eabi include folders
INC_DIRS += /usr/arm-none-eabi/include
INC_DIRS += /usr/arm-none-eabi/include/c++/8.2.0
INC_DIRS += /usr/arm-none-eabi/include/c++/8.2.0/arm-none-eabi/thumb/v7e-m/nofp
#INC_DIRS += /usr/arm-none-eabi/include/c++/8.2.0/arm-none-eabi/thumb/v7e-m+fp/hard/
INC_DIRS += /usr/arm-none-eabi/include/c++/8.2.0/backward
INC_DIRS += /usr/lib/gcc/arm-none-eabi/8.2.0/include
INC_DIRS += /usr/lib/gcc/arm-none-eabi/8.2.0/include-fixed

# Create include flags
INC_FLAGS = $(addprefix -I,$(INC_DIRS))

# Preprocessor flags
CPPFLAGS += $(INC_FLAGS) -MMD -MP

# Common flags
FLAGS += $(VFLAG) 
FLAGS += -march=armv7e-m -mcpu=cortex-m4 -mthumb -mlittle-endian
#FLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
FLAGS += -fmessage-length=0 -fsigned-char
FLAGS += --sysroot=/usr/arm-none-eabi/
FLAGS += $(OPTFLAGS)
FLAGS += $(WARNFLAGS)
FLAGS += $(DBGFLAGS)

# Assembler flags
ASFLAGS += $(FLAGS)
ASFLAGS += $(INC_FLAGS)
ASFLAGS += -x assembler-with-cpp

# C flags
CFLAGS += $(FLAGS)
CFLAGS += -std=gnu11

# C++ flags
CXXFLAGS += $(FLAGS)
CXXFLAGS += -std=c++2a -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics -ftemplate-depth=2048 -ftemplate-backtrace-limit=0 -fconcepts

# Linker flags
LDFLAGS += $(FLAGS)
LDFLAGS += -nostartfiles
LDFLAGS += --specs=nano.specs -Wl,--gc-sections,-Map=$(MAP)