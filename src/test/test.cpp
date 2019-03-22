#include <cassert>
#include <cinttypes>
#include <cstdio>
#include <cstring>
#include <functional>
#include <tuple>
#include <type_traits>
#include "bench.h"
#include "forth2012_test_suite.hpp"
#include "main.h"
#include "shi.hpp"

using shi::operator""_s;
using shi::operator""_w;

void semihosting_example();

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

  // die variante geht nicht???
  ": acker "
  "over 0= if nip 1+ exit then "
  "swap 1- swap "
  "dup 0= if 1+ recurse exit then "
  "1- over 1+ swap recurse recurse ;"_s;

  asm volatile("nop");
  "3 4"_s;
  asm volatile("nop");
  "acker"_s;

  asm volatile("nop");
  stack_dump();

  // die aber scho? hä?
  ": ackermann "
  "over "
  "0 over = if drop nip 1+     else "
  "1 over = if drop nip 2 +    else "
  "2 over = if drop nip 2* 3 + else "
  "3 over = if drop swap 5 + swap lshift 3 - else "
  "  drop swap 1- swap dup "
  "  if "
  "    1- over 1+ swap recurse recurse exit "
  "  else "
  "    1+ recurse exit "
  "  then "
  "then then then then "
  ";"_s;

  asm volatile("nop");
  "3 4"_s;
  asm volatile("nop");
  "ackermann"_s;

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

  bench();

  // semihosting_example();

  // return forth2012_test_suite();
  return 0;
}
