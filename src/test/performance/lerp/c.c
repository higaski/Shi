#include "dwt_cyccnt.h"
#include "stm32f4xx_hal.h"

static int lerp(int x, int x1, int x2, int y1, int y2) {
  return y1 + ((y2 - y1) * (x - x1)) / (x2 - x1);
}

void test_performance_c() {}
