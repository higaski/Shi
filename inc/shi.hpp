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

void init(uint32_t const ram_begin, uint32_t const ram_end);
void clear();
bool empty();
size_t size();
void pop();
void evaluate(char const* str);
void evaluate(char const* str, size_t const len);
int stack_print(int (*fp)(char const* const format, ...));

/// Returns the first element of the stack
///
/// \warning
/// Be aware that the template parameter defaults to integer values. Calling the
/// function with other types compiles, but returns wrong values if not
/// explicitly specialized.
/// ```
/// float f{top()};    // wrong retval float
/// f = top<float>();  // right retval
/// ```
///
/// \tparam   T   Type to read from top
/// \return   Top of stack
template<typename T = int32_t>
T top() {
  static_assert(sizeof(T) <= 4, "'shi::top()' type T size <= 4");

  T cell;

  asm volatile("ldr %[cell], =_s_shi_context"
               "\n\t"
               "ldr %[cell], [%[cell]]"
               "\n\t"
               ".ltorg"
               "\n\t"
               : [cell] "=r"(cell));

  return cell;
}

/// Returns the first element of the stack
///
/// \tparam   T       Type to read from top
/// \param    cell    Top of stack
template<typename T>
void top(T& cell) {
  asm volatile("ldr %[cell], =_s_shi_context"
               "\n\t"
               "ldr %[cell], [%[cell]]"
               "\n\t"
               ".ltorg"
               "\n\t"
               : [cell] "=r"(cell));
}

/// Add element to the top of the stack
///
/// \tparam   T       Type to push
/// \param    cell    Value to push
template<typename T>
void push(T const cell) {
  static_assert(sizeof(T) <= 4, "'shi::push(T const)' type T size <= 4");

  asm volatile("ldr r0, =_s_shi_context"
               "\n\t"
               "ldmia r0, {r1, r2}"
               "\n\t"
               "str r1, [r2, #-4]!"
               "\n\t"
               "movs r1, %[cell]"
               "\n\t"
               "stmia r0, {r1, r2}"
               "\n\t"
               ".ltorg"
               "\n\t"
               :
               : [cell] "r"(cell)
               : "cc", "r0", "r1", "r2");
}

template<>
void push<float>(float const cell);

// ASM prototypes
extern "C" void shi_init(uint32_t, uint32_t);
extern "C" void shi_c_variable(char const* name, size_t len);
extern "C" void shi_evaluate(char const* str, size_t len);

template<typename T>
void c_variable(char const* name, T adr) {
  static_assert(std::is_pointer_v<T>,
                "'shi::c_variable(char const*, T)' type T is pointer");
  push(adr);
  shi_c_variable(name, std::strlen(name));
}

}  // namespace shi
