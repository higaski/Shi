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

  //  asm volatile("nop");
  //  "17 create SEVENTEEN ,"_fs;
  //  asm volatile("nop");
  //  printf("%d\n", shi::size());
  //  "SEVENTEEN"_fs;
  //  asm volatile("nop");
  //  printf("%d\n", shi::size());
  //  if (shi::size()) {
  //    printf("%d\n", shi::top());
  //    printf("%d\n", *reinterpret_cast<uint32_t*>(shi::top()));
  //  }

  asm volatile("nop");
  ": my_constant create , does> @ ;"_fs;
  // geht des a?
  // "my_constant_no_colon create , does> @"_fs;
  printf("%d\n", shi::size());
  asm volatile("nop");
  "1337 my_constant l33t"_fs;
  asm volatile("nop");
  printf("%d\n", shi::size());
  "l33t"_fs;
  asm volatile("nop");
  if (shi::size()) {
    printf("%d\n", shi::top());
    printf("%d\n", *reinterpret_cast<uint32_t*>(shi::top()));
  }

  asm volatile("nop");
  ": indexed-array create cells allot does> swap cells + ;"_fs;
  asm volatile("nop");
  "20 indexed-array foo"_fs;
  asm volatile("nop");

  // test_performance_shi();
  // test_performance_lua();
  // test_performance_micropython();

  // semihosting_example();

  // return forth2012_test_suite();
  printf("done\n");
  return 0;
}
