// https://github.com/gerryjackson/forth2012-test-suite/tree/master/src

#pragma once

#include "unity.h"

#define TEST(name) void name()

#define _0S 0
#define _1S -1
#define MSB (1 << 31)
#define MAX_UINT 4294967295ul
#define MAX_INT 2147483647
#define MIN_INT -2147483648
#define MID_UINT 2147483647ul
#define MID_UINT_p1 2147483648ul
#define FALSE 0
#define TRUE -1

int forth2012_test_suite();

#ifdef __cplusplus

#  include <utility>
#  include "shi.hpp"

namespace detail {

template<typename... Ts, size_t... Is>
void stack_equal_impl(std::index_sequence<Is...>, Ts... ts) {
  (TEST_ASSERT_EQUAL(ts, shi::top(Is)), ...);
}

}  // namespace detail

template<typename... Ts>
void stack_equal(Ts... ts) {
  detail::stack_equal_impl(std::make_index_sequence<sizeof...(Ts)>{}, ts...);
}

#endif
