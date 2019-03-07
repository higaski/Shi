// https://github.com/gerryjackson/forth2012-test-suite/tree/master/src

#pragma once

#include "shi.hpp"
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
