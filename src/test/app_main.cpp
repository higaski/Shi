#include <array>
#include "bench.h"
#include "forth2012_test_suite.hpp"

void semihosting_example();

void stack_dump() {
  printf("stack dump:\n");
  for (auto i{0u}; i < shi::size(); ++i)
    printf("%d. on stack: %d    %X\n", i, shi::top(i), shi::top(i));
}

extern "C" int app_main() {
  //  using namespace shi;
  //  printf("%X\n", &shi_ram[0]);
  //  ": indexed-array create cells allot does> swap cells + ;"_s;
  //  "20 indexed-array fo"_s;
  //  stack_dump();
  //  shi::push(42, 3);
  //  "fo !"_s;
  //  stack_dump();

  //  asm volatile("nop");
  //  ": indexed-array create cells allot does> swap cells + ;"_s;
  //  asm volatile("nop");
  //  "20 indexed-array foo"_s;
  //  asm volatile("nop");

  // semihosting_example();

#if defined(APP_MAIN_TEST)
  return forth2012_test_suite();
#elif defined(APP_MAIN_BENCH)
  return bench();
#endif
}
