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

extern "C" void performance_mp_lerp();

void stack_dump() {
  printf("stack dump:\n");
  for (auto i{0u}; i < shi::size(); ++i)
    printf("%d. on stack: %d\n", i, shi::top(i));
}

extern "C" int test() {
  shi::init({.data_begin = SHI_RAM_START,
             .data_end = SHI_RAM_END,
             .text_begin = SHI_FLASH_START,
             .text_end = SHI_FLASH_END});

  ": GI2 if 123 else 234 then ;"_s;
  "0 GI2"_s;  // 234
  stack_dump();

  "1 GI2"_s;  // 123
  stack_dump();

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
  // performance_mp_lerp();

  // semihosting_example();

  // shi::clear();

  //  return forth2012_test_suite();
  return 0;
}
