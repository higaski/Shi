#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int32_t x[2000];
  int32_t x1[2000];
  int32_t x2[2000];
  int32_t y1[2000];
  int32_t y2[2000];
  int32_t y[2000];
} lerp_data_t;

extern lerp_data_t const lerp_data;

#ifdef __cplusplus
}
#endif
