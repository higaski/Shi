#include <cstdio>
#include "forth2012_test_suite.hpp"
#include "shi.hpp"

using shi::operator""_fs;

void semihosting_example();

extern "C" void test_performance_shi();
extern "C" void test_performance_lua();
extern "C" void test_performance_micropython();

extern "C" int test() {
  //  shi::init({.data_begin = SHI_RAM_START,
  //             .data_end = SHI_RAM_END,
  //             .text_begin = SHI_FLASH_START,
  //             .text_end = SHI_FLASH_END});

  // test_performance_shi();
  // test_performance_lua();
  // test_performance_micropython();

  //  semihosting_example();

  // return forth2012_test_suite();
  return 0;
}
