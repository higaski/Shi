#include "bench.h"
#include "forth2012_test_suite.hpp"
#include "quick_n_dirty.hpp"

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
#elif defined(APP_MAIN_QUICK_N_DIRTY)
  return (quick_n_dirty(), 0);
#endif
}
