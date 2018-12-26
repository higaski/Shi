/// Shi
///
/// \file       shi.hpp
/// \author     Vincent Hamp
/// \date       27/07/2016

/// \page page_dictionary Dictionary
///
/// \page page_api API
///
/// http://forth-standard.org/
///
///
///
/// \page page_implementation Implementation
/// sd
///
/// \section section_stack_notation Stack notation
/// blabla

#pragma once

// Assembler only
#ifdef __ASSEMBLER__

/// Data-stack size in bytes
#  define SHI_DSTACK_SIZE 256

/// Most flash types are 0xFF erased
#  define SHI_ERASED_WORD 0xFFFFFFFF

/// If defined enables the prefixes $, # and % for numbers
#  define SHI_NUMBER_PREFIX 1

/// If defined, printing messages is enabled and used for errors
#  define SHI_PRINT_ENABLED 1

// C/C++
#else

#  include <stddef.h>
#  include <stdint.h>
#  include <string.h>

typedef struct {
  uint32_t data_begin;
  uint32_t data_end;
  uint32_t text_begin;
  uint32_t text_end;
} shi_init_t;

#  ifdef __cplusplus
extern "C" {
#  endif

void shi_init_asm(uint32_t data_begin,
                  uint32_t data_end,
                  uint32_t text_begin,
                  uint32_t text_end);
void shi_c_variable_asm(char const* name, size_t len);
void shi_evaluate_asm(char const* str, size_t len);

#  ifdef __cplusplus
}
#  endif

// C only
#  ifndef __cplusplus

inline void shi_init(shi_init_t s) {
  shi_init_asm(s.data_begin, s.data_end, s.text_begin, s.text_end);
}

inline void shi_evaluate(char const* str) {
  shi_evaluate_asm(str, strlen(str));
}

inline void shi_evaluate_len(char const* str, size_t len) {
  shi_evaluate_asm(str, len);
}

inline void shi_cvariable(char const* str, void* adr) {
  push(reinterpret_cast<int32_t>(adr));
  shi_c_variable_asm(str, strlen(name));
}

// C++ only
#  else

#    include <type_traits>

namespace shi {

void init(shi_init_t s);
void clear();
bool empty();
size_t depth();
size_t size();
void push(int32_t cell);
void pop();
int32_t top(int32_t offset = 0);
void evaluate(char const* str);
void evaluate(char const* str, size_t len);
int stack_print(int (*fp)(char const* const format, ...));

inline void operator"" _fs(char const* str, size_t len) {
  shi_evaluate_asm(str, len);
}

template<typename T>
void cvariable(char const* str, T adr) {
  static_assert(std::is_pointer_v<T>);
  static_assert(std::is_integral_v<std::remove_pointer_t<T>>);

  push(reinterpret_cast<int32_t>(adr));
  shi_c_variable_asm(str, strlen(str));
}

}  // namespace shi

#  endif

#endif
