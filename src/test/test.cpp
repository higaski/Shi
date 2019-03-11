#include <cstdio>
#include "forth2012_test_suite.hpp"
#include "main.h"
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

  //"!"_fs;

  auto retval = forth2012_test_suite();
//
//  while (1) {
//    HAL_Delay(10);
//    HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_SET);
//    //"' CS7"_fs;
//    "' c-variable"_fs;  // ziemlich weit hint im flash...
//    HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_RESET);
//    shi::clear();
//  }

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
