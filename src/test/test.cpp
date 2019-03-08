#include <cstdio>
#include "forth2012_test_suite.hpp"
#include "shi.hpp"

using shi::operator""_fs;

void semihosting_example();

extern "C" void test_performance_shi();
extern "C" void test_performance_lua();
extern "C" void test_performance_micropython();

extern "C" int test() {
  shi::init({.data_begin = SHI_RAM_START,
             .data_end = SHI_RAM_END,
             .text_begin = SHI_FLASH_START,
             .text_end = SHI_FLASH_END});

  ": GD2 do i 2 = if 2 leave then i -2 = if -2 leave then loop ;"_fs;
//  "4 1"_fs;
//  "GD2"_fs;
  printf("size %d  top %d\n", shi::size(), shi::top());
  "-1 -5"_fs;
  "GD2"_fs;
  printf("size %d  top %d\n", shi::size(), shi::top());

  //  asm volatile("nop");
  //  ": indexed-array create cells allot does> swap cells + ;"_fs;
  //  asm volatile("nop");
  //  "20 indexed-array foo"_fs;
  //  asm volatile("nop");

  // test_performance_shi();
  // test_performance_lua();
  // test_performance_micropython();

  // semihosting_example();

  // return forth2012_test_suite();
  return 0;
}
