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

using shi::operator""_s, shi::operator""_w;

void semihosting_example();

void stack_dump() {
  printf("stack dump:\n");
  for (auto i{0u}; i < shi::size(); ++i)
    printf("%d. on stack: %d\n", i, shi::top(i));
}

alignas(4) std::array<uint8_t, 32 * 1024> shi_ram{};

extern "C" int test() {
  using std::begin, std::end;

  shi::init({.data_begin = reinterpret_cast<uint32_t>(begin(shi_ram)),
             .data_end = reinterpret_cast<uint32_t>(end(shi_ram)),
             .text_begin = FLASH_END - (32 * 1024),
             .text_end = FLASH_END});

  //  "2 allot"_s;

  //  printf("begin shi_ram %X\n", reinterpret_cast<uint32_t>(begin(shi_ram)));

  // this never worked
  // endcase currently also resolved the branch from leave
  // which means when we leave we land right before "loop" which pops the loop
  // control parameters a SECOND TIME
  //  ": CS do i case 1 of leave endof 2 of 222 endof 3 of 333 endof endcase
  //  loop ;"_s; shi::push(4, 1); printf("%d\n", shi::size()); "CS"_s;
  //  printf("%d\n", shi::size());

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

  // bench();

  // semihosting_example();

  return forth2012_test_suite();
}
