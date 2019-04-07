#include <array>
#include "bench.h"
#include "forth2012_test_suite.hpp"
#include "main.h"
#include "shi.hpp"

void semihosting_example();

alignas(4) std::array<uint8_t, 32 * 1024> shi_ram{};

void stack_dump() {
  printf("stack dump:\n");
  for (auto i{0u}; i < shi::size(); ++i)
    printf("%d. on stack: %d    %X\n", i, shi::top(i), shi::top(i));
}

extern "C" int test() {
  shi::init({.data_begin = reinterpret_cast<uint32_t>(begin(shi_ram)),
             .data_end = reinterpret_cast<uint32_t>(end(shi_ram)),
             .text_begin = FLASH_END - (32 * 1024),
             .text_end = FLASH_END});

  using namespace shi;
  printf("%X\n", &shi_ram[0]);
  ": indexed-array create cells allot does> swap cells + ;"_s;
  "20 indexed-array fo"_s;
  stack_dump();
  shi::push(42, 3);
  "fo !"_s;
  stack_dump();

  //  asm volatile("nop");
  //  ": indexed-array create cells allot does> swap cells + ;"_s;
  //  asm volatile("nop");
  //  "20 indexed-array foo"_s;
  //  asm volatile("nop");

  // bench();

  // semihosting_example();

  // return forth2012_test_suite();
  return 0;
}
