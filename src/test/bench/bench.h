#pragma once

#include "dwt_cyccnt.h"

void bench();

#define bench_assert(__e)                                                      \
  ((__e) ? (void)0 : bench_assert_func(__FILE__, __LINE__, __func__, #__e))

#ifdef __cplusplus
extern "C" {
#endif

void bench_assert_func(char const* file,
                       int line,
                       char const* func,
                       char const* failedexpr);

#ifdef __cplusplus
}
#endif
