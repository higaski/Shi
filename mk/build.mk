# Recursively add all sources
SRCS := $(shell find $(SRC_DIRS) -name *asm -or -name *.s -or -name *.S -or -name *.c -or -name *.cc -or -name *.cpp)
OBJS := $(SRCS:%=$(TARGET_DIR)/%.o)

# Include all dependency files (.d)
DEPS := $(OBJS:.o=.d)

# Create include directories from dependency files
INC_DIRS += $(shell find $(SRC_DIRS) -type d)

# Create executable
$(EXE): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS)  

# Create hex
$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@
	$(OBJDUMP) --source --all-headers --demangle --line-numbers --wide $(ELF) > $(LST)
	$(SIZE) --format=berkeley $(ELF)

# Create elf
$(ELF): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS) 

# Create lib
$(LIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

# Assembly
$(TARGET_DIR)/%.asm.o: %.asm
	$(MKDIR_P) $(dir $@)
	$(AS) $(CPPFLAGS) $(ASFLAGS) -c $< -o $@	

$(TARGET_DIR)/%.s.o: %.s
	$(MKDIR_P) $(dir $@)
	$(AS) $(CPPFLAGS) $(ASFLAGS) -c $< -o $@
	
$(TARGET_DIR)/%.S.o: %.S
	$(MKDIR_P) $(dir $@)
	$(AS) $(CPPFLAGS) $(ASFLAGS) -c $< -o $@	

# C 
$(TARGET_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# C++ 
$(TARGET_DIR)/%.cc.o: %.cc
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(TARGET_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

-include $(DEPS)