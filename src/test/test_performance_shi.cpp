#include <cstdio>
#include "dwt_cyccnt.h"
#include "shi.hpp"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

using shi::operator""_fs;

extern "C" void test_performance_shi() {

  // lerp ( x x1 x2 y1 y2 -- y )
  ": lerp over - 4 roll 4 pick - * 2swap swap - / + ;"_fs;

  __disable_irq();
  uint32_t it1, it2;
  DWT_CYCCNT_EN();
  it1 = DWT_CYCCNT_GET();

  shi::push(5);
  shi::push(1);
  shi::push(10);
  shi::push(5);
  shi::push(50);
  "lerp"_fs;
  int result = shi::top();
  shi::pop();

  it2 = DWT_CYCCNT_GET() - it1;
  DWT_CYCCNT_DIS();
  __enable_irq();
  printf("result: %d\n", result);
  printf("%d\n", it2);
  HAL_Delay(1000);
}
