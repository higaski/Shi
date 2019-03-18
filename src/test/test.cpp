#include <cassert>
#include <cinttypes>
#include <cstdio>
#include <cstring>
#include <functional>
#include <tuple>
#include <type_traits>
#include "forth2012_test_suite.hpp"
#include "main.h"
#include "shi.hpp"

using shi::operator""_s;

void semihosting_example();

extern "C" void test_performance_shi();
extern "C" void test_performance_lua();
extern "C" void test_performance_micropython();

namespace shi {

struct word {
  constexpr word() = default;
  word(char const* str) : fp{shi_tick_asm(str, strlen(str))} {}
  word(char const* str, size_t len) : fp{shi_tick_asm(str, len)} {}

  template<typename... Ts>
  word& operator()(Ts&&... ts) {
    using std::forward;

    (push(forward<Ts>(ts)), ...);

    asm volatile("ldmia %0, {r6, r7, r8} \n"
                 "push {%0} \n"
                 "blx %1 \n"
                 "pop {%0} \n"
                 "stmia %0, {r6, r7, r8} \n"
                 :
                 : "r"(&s_shi_context), "r"(fp)
                 : "cc", "memory", "r6", "r7", "r8", "lr");

    return *this;
  }

  template<typename T>
  operator T() {
    return pop<T>();
  }

  template<typename... Ts>
  operator std::tuple<Ts...>() {
    return std::tuple<Ts...>{(pop<Ts>(), ...)};
  }

private:
  void_fp fp{nullptr};
};

template<typename T, T... Cs>
word operator""_w() {
  static constexpr char c[]{Cs...};
  static auto literal_word{word(c, sizeof...(Cs))};
  return literal_word;
}

}  // namespace shi

void stack_dump() {
  printf("stack dump:\n");
  for (auto i{0u}; i < shi::size(); ++i)
    printf("%d. on stack: %X\n", i, shi::top(i));
}

extern "C" int test() {
  shi::init({.data_begin = SHI_RAM_START,
             .data_end = SHI_RAM_END,
             .text_begin = SHI_FLASH_START,
             .text_end = SHI_FLASH_END});

  using shi::operator""_w;

  "wtf"_s;
  asm volatile("nop");

  asm volatile("nop");
  ": seven 7 ;"_s;
  auto seven = shi::word("seven");
  asm volatile("nop");
  printf("%d\n", shi::size());
  asm volatile("nop");
  seven();
  asm volatile("nop");
  printf("%d\n", shi::size());
  asm volatile("nop");
  printf("%d\n", shi::top());

  asm volatile("nop");
  "seven"_w();
  asm volatile("nop");
  printf("%d\n", shi::size());
  asm volatile("nop");
  printf("%d\n", shi::top());

  //  shi::push(1);
  //  shi::push(2);
  //  shi::push(3);
  //  shi::push(4);
  //
  //  stack_dump();
  //  printf("\n");
  //
  //  shi::pop();
  //
  //  stack_dump();
  //  printf("\n");
  //
  //  int i = shi::pop<int>();
  //  printf("value popped: %d\n", i);
  //
  //  stack_dump();
  //
  //  printf("\n");
  //
  //  uint64_t big = 0xAAAABBBBCCCCDDDD;
  //  shi::push(big);
  //  stack_dump();
  //  printf("\n");
  //
  //  asm volatile("nop");
  //  uint64_t big_popped = shi::pop<uint64_t>();
  //  printf("%zu\n", big_popped);
  //  stack_dump();
  //  printf("\n");
  //
  //  assert(big == big_popped);

  //  ": add1 1+ ;"_s;
  //  auto add1 = shi::word("add1");
  //  asm volatile("nop");
  //  int retval = add1(7);
  //  printf("%d\n", retval);
  // add1(7);

  //  asm volatile("nop");
  //  ": indexed-array create cells allot does> swap cells + ;"_s;
  //  asm volatile("nop");
  //  "20 indexed-array foo"_s;
  //  asm volatile("nop");

  // test_performance_shi();
  // test_performance_lua();
  // test_performance_micropython();

  // semihosting_example();

  shi::clear();

  return forth2012_test_suite();
}
