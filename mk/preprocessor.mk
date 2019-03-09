# Enable assertions
ifneq ($(DEBUG),1)
	CPPFLAGS += -DNDEBUG
endif

# Enable trace debugging
ifeq ($(TRACE),1)
	CPPFLAGS += -DTRACE 
	CPPFLAGS += -DOS_USE_TRACE_SEMIHOSTING_STDOUT
	LDFLAGS += -u _printf_float
endif