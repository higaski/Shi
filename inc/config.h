//#ifdef __ASSEMBLER__

/// Data-stack size in bytes
#  define SHI_DSTACK_SIZE 256

/// Most flash types are 0xFF erased
#  define SHI_ERASED_WORD 0xFFFFFFFF

/// If defined enables the prefixes $, # and % for numbers
#  define SHI_NUMBER_PREFIX 1

/// If defined, trace write is enabled and used for errors
#  define SHI_TRACE_ENABLED 1

//#endif
