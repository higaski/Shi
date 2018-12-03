/// Fumb-2
///
/// \file       fumb-2.hpp
/// \author     Vincent Hamp
/// \date       27/07/2016

#pragma once

#include <cstdint>
#include <cstring>
#include <type_traits>

namespace shi {

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

void init(uint32_t const ram_begin,
          uint32_t const ram_end,
          uint32_t const flash_begin,
          uint32_t const flash_end);
void clear();
bool empty();
size_t depth();
size_t size();
void push(int32_t const cell);
void pop();
int32_t top(int32_t const offset = 0);
void evaluate(char const* str);
void evaluate(char const* str, size_t const len);
int stack_print(int (*fp)(char const* const format, ...));

// ASM prototypes
extern "C" {

void shi_init(uint32_t const ram_begin,
              uint32_t const ram_end,
              uint32_t const flash_begin,
              uint32_t const flash_end);
void shi_c_variable(char const* name, size_t len);
void shi_evaluate(char const* str, size_t len);

}

inline void operator"" _fs(char const* str, size_t const len) {
  evaluate(str, len);
}

template<typename T>
void c_variable(char const* name, T adr) {
  static_assert(std::is_pointer_v<T>,
                "'shi::c_variable(char const*, T)' type T is pointer");
  push(reinterpret_cast<uint32_t>(adr));
  shi_c_variable(name, std::strlen(name));
}

}  // namespace shi
