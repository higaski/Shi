// https://github.com/gerryjackson/forth2012-test-suite/tree/master/src

#pragma once

#include "shi.hpp"
#include "unity.h"

#define TEST(name) void name()

#ifdef __cplusplus

#  include <limits>

#  define TEST(name) extern "C" void name()

inline constexpr auto _0S{0};
inline constexpr auto _1S{-1};
inline constexpr auto MSB{1 << 31};
inline constexpr auto FALSE{0};
inline constexpr auto TRUE{-1};
inline constexpr auto MIN_INT{std::numeric_limits<int32_t>::min()};
inline constexpr auto MAX_INT{std::numeric_limits<int32_t>::max()};
inline constexpr auto MAX_UINT{std::numeric_limits<uint32_t>::max()};
inline constexpr auto MID_UINT{std::numeric_limits<int32_t>::max()};
inline constexpr uint32_t MID_UINT_p1{MID_UINT + 1u};

int forth2012_test_suite();

#endif
