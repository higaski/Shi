#include <cstdio>
#include "shi.hpp"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "unity.h"

#define SHI_FLASH_START (0x08000000 + 0x00040000 - 0x8000)
#define SHI_FLASH_END (0x08000000 + 0x00040000)
#define SHI_RAM_START (0x20004000)
#define SHI_RAM_END (0x20004000 + 0x00004000)

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

using shi::operator""_fs;

void shi_test();
void shi_test_compile_to_flash();
void SystemClock_Config();
void _Error_Handler(char* file, int line);

// https://github.com/gerryjackson/forth2012-test-suite/tree/master/src

#define TEST(name) void name()

TEST(basic_assumptions) {
  ": BITSSET? if 0 0 else 0 then ;"_fs;

  "0 BITSSET?"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::size());
  TEST_ASSERT_EQUAL_INT(0, shi::top());

  "1 BITSSET?"_fs;
  TEST_ASSERT_EQUAL_INT(3, shi::size());
  TEST_ASSERT_EQUAL_INT(0, shi::top());
  TEST_ASSERT_EQUAL_INT(0, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(0, shi::top(-2));

  "-1 BITSSET?"_fs;
  TEST_ASSERT_EQUAL_INT(5, shi::size());
  TEST_ASSERT_EQUAL_INT(0, shi::top());
  TEST_ASSERT_EQUAL_INT(0, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(0, shi::top(-2));
  TEST_ASSERT_EQUAL_INT(0, shi::top(-3));
  TEST_ASSERT_EQUAL_INT(0, shi::top(-4));

  shi::clear();
}

TEST(invert__and__or__xor) {
  "0 0 and"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::top());
  "0 1 and"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::top());
  "1 0 and"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::top());
  "1 1 and"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::top());
  "0 invert 1 and"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::top());
  "1 invert 1 and"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::top());

  "0 constant 0S"_fs;
  "0 invert constant 1S"_fs;

  "0S invert"_fs;
  TEST_ASSERT_EQUAL_INT(_1S, shi::top());
  "1S invert"_fs;
  TEST_ASSERT_EQUAL_INT(_0S, shi::top());

  "0S 0S or"_fs;
  TEST_ASSERT_EQUAL_INT(_0S, shi::top());
  "0S 1S or"_fs;
  TEST_ASSERT_EQUAL_INT(_1S, shi::top());
  "1S 0S or"_fs;
  TEST_ASSERT_EQUAL_INT(_1S, shi::top());
  "1S 1S or"_fs;
  TEST_ASSERT_EQUAL_INT(_1S, shi::top());

  "0S 0S xor"_fs;
  TEST_ASSERT_EQUAL_INT(_0S, shi::top());
  "0S 1S xor"_fs;
  TEST_ASSERT_EQUAL_INT(_1S, shi::top());
  "1S 0S xor"_fs;
  TEST_ASSERT_EQUAL_INT(_1S, shi::top());
  "1S 1S xor"_fs;
  TEST_ASSERT_EQUAL_INT(_0S, shi::top());

  shi::clear();
}

TEST(two_times__two_div__lshift__rshit) {
  "1S 1 rshift invert constant MSB"_fs;
  "MSB BITSSET?"_fs;
  TEST_ASSERT_EQUAL_INT(2, shi::size());
  TEST_ASSERT_EQUAL_INT(0, shi::top());
  TEST_ASSERT_EQUAL_INT(0, shi::top(-1));

  "0S 2*"_fs;
  TEST_ASSERT_EQUAL_INT(_0S, shi::top());
  "1 2*"_fs;
  TEST_ASSERT_EQUAL_INT(2, shi::top());
  "4000 2*"_fs;
  TEST_ASSERT_EQUAL_INT(8000, shi::top());
  "1S 2* 1 xor"_fs;
  TEST_ASSERT_EQUAL_INT(_1S, shi::top());
  "MSB 2*"_fs;
  TEST_ASSERT_EQUAL_INT(_0S, shi::top());

  "0S 2/"_fs;
  TEST_ASSERT_EQUAL_INT(_0S, shi::top());
  "1 2/"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::top());
  "4000 2/"_fs;
  TEST_ASSERT_EQUAL_INT(2000, shi::top());
  "1S 2/"_fs;
  TEST_ASSERT_EQUAL_INT(_1S, shi::top());
  "1S 1 xor 2/"_fs;
  TEST_ASSERT_EQUAL_INT(_1S, shi::top());
  "MSB 2/ MSB and"_fs;
  TEST_ASSERT_EQUAL_INT(MSB, shi::top());

  "1 0 lshift"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::top());
  "1 1 lshift"_fs;
  TEST_ASSERT_EQUAL_INT(2, shi::top());
  "1 2 lshift"_fs;
  TEST_ASSERT_EQUAL_INT(4, shi::top());
  //  shi::evaluate("1 F lshift");  // -> 8000 }T         \ BIGGEST GUARANTEED
  //  SHIFT TEST_ASSERT_EQUAL_INT(8000, shi::top());
  "1S 1 lshift 1 xor"_fs;
  TEST_ASSERT_EQUAL_INT(_1S, shi::top());
  "MSB 1 lshift"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::top());

  "1 0 rshift"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::top());
  "1 1 rshift"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::top());
  "2 1 rshift"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::top());
  "4 2 rshift"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::top());
  // shi::evaluate("8000 F rshift");  // -> 1 }T         \ BIGGEST
  //  TEST_ASSERT_EQUAL_INT(1, shi::top());
  "MSB 1 rshift MSB and"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::top());
  "MSB 1 rshift 2*"_fs;
  TEST_ASSERT_EQUAL_INT(MSB, shi::top());

  shi::clear();
}

TEST(zero_equal__equal__zero_less__less__more__u_less__min__max) {
  "0 invert constant MAX-UINT"_fs;
  "0 invert 1 rshift constant MAX-INT"_fs;
  "0 invert 1 rshift invert constant MIN-INT"_fs;
  "0 invert 1 rshift constant MID-UINT"_fs;
  "0 invert 1 rshift invert constant MID-UINT+1"_fs;
  "0S constant <FALSE>"_fs;
  "1S constant <TRUE>"_fs;

  "0 0="_fs;
  TEST_ASSERT_EQUAL_INT(TRUE, shi::top());
  "1 0="_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "2 0="_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "-1 0="_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "MAX-UINT 0="_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "MIN-INT 0="_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "MAX-INT 0="_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());

  "0 0 ="_fs;
  TEST_ASSERT_EQUAL_INT(TRUE, shi::top());
  "1 1 ="_fs;
  TEST_ASSERT_EQUAL_INT(TRUE, shi::top());
  "-1 -1 ="_fs;
  TEST_ASSERT_EQUAL_INT(TRUE, shi::top());
  "1 0 ="_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "-1 0 ="_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "0 1 ="_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "0 -1 ="_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());

  "0 0<"_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "-1 0<"_fs;
  TEST_ASSERT_EQUAL_INT(TRUE, shi::top());
  "MIN-INT 0<"_fs;
  TEST_ASSERT_EQUAL_INT(TRUE, shi::top());
  "1 0<"_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "MAX-INT 0<"_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());

  "0 1 <"_fs;
  TEST_ASSERT_EQUAL_INT(TRUE, shi::top());
  "1 2 <"_fs;
  TEST_ASSERT_EQUAL_INT(TRUE, shi::top());
  "-1 0 <"_fs;
  TEST_ASSERT_EQUAL_INT(TRUE, shi::top());
  "-1 1 <"_fs;
  TEST_ASSERT_EQUAL_INT(TRUE, shi::top());
  "MIN-INT 0 <"_fs;
  TEST_ASSERT_EQUAL_INT(TRUE, shi::top());
  "MIN-INT MAX-INT <"_fs;
  TEST_ASSERT_EQUAL_INT(TRUE, shi::top());
  "0 MAX-INT <"_fs;
  TEST_ASSERT_EQUAL_INT(TRUE, shi::top());
  "0 0 <"_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "1 1 <"_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "1 0 <"_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "2 1 <"_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "0 -1 <"_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "1 -1 <"_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "0 MIN-INT <"_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "MAX-INT MIN-INT <"_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "MAX-INT 0 <"_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());

  "0 1 >"_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "1 2 >"_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "-1 0 >"_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "-1 1 >"_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "MIN-INT 0 >"_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "MIN-INT MAX-INT >"_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "0 MAX-INT >"_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "0 0 >"_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "1 1 >"_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "1 0 >"_fs;
  TEST_ASSERT_EQUAL_INT(TRUE, shi::top());
  "2 1 >"_fs;
  TEST_ASSERT_EQUAL_INT(TRUE, shi::top());
  "0 -1 >"_fs;
  TEST_ASSERT_EQUAL_INT(TRUE, shi::top());
  "1 -1 >"_fs;
  TEST_ASSERT_EQUAL_INT(TRUE, shi::top());
  "0 MIN-INT >"_fs;
  TEST_ASSERT_EQUAL_INT(TRUE, shi::top());
  "MAX-INT MIN-INT >"_fs;
  TEST_ASSERT_EQUAL_INT(TRUE, shi::top());
  "MAX-INT 0 >"_fs;
  TEST_ASSERT_EQUAL_INT(TRUE, shi::top());

  shi::clear();

  "0 1 u<"_fs;
  TEST_ASSERT_EQUAL_INT(TRUE, shi::top());
  "1 2 u<"_fs;
  TEST_ASSERT_EQUAL_INT(TRUE, shi::top());
  "0 MID-UINT u<"_fs;
  TEST_ASSERT_EQUAL_INT(TRUE, shi::top());
  "0 MAX-UINT u<"_fs;
  TEST_ASSERT_EQUAL_INT(TRUE, shi::top());
  "MID-UINT MAX-UINT u<"_fs;
  TEST_ASSERT_EQUAL_INT(TRUE, shi::top());
  "0 0 u<"_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "1 1 u<"_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "1 0 u<"_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "2 1 u<"_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "MID-UINT 0 u<"_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "MAX-UINT 0 u<"_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());
  "MAX-UINT MID-UINT u<"_fs;
  TEST_ASSERT_EQUAL_INT(FALSE, shi::top());

  "0 1 min"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::top());
  "1 2 min"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::top());
  "-1 0 min"_fs;
  TEST_ASSERT_EQUAL_INT(-1, shi::top());
  "-1 1 min"_fs;
  TEST_ASSERT_EQUAL_INT(-1, shi::top());
  "MIN-INT 0 min"_fs;
  TEST_ASSERT_EQUAL_INT(MIN_INT, shi::top());
  "MIN-INT MAX-INT min"_fs;
  TEST_ASSERT_EQUAL_INT(MIN_INT, shi::top());
  "0 MAX-INT min"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::top());
  "0 0 min"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::top());
  "1 1 min"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::top());
  "1 0 min"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::top());
  "2 1 min"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::top());
  "0 -1 min"_fs;
  TEST_ASSERT_EQUAL_INT(-1, shi::top());
  "1 -1 min"_fs;
  TEST_ASSERT_EQUAL_INT(-1, shi::top());
  "0 MIN-INT min"_fs;
  TEST_ASSERT_EQUAL_INT(MIN_INT, shi::top());
  "MAX-INT MIN-INT min"_fs;
  TEST_ASSERT_EQUAL_INT(MIN_INT, shi::top());
  "MAX-INT 0 min"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::top());

  "0 1 max"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::top());
  "1 2 max"_fs;
  "-1 0 max"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::top());
  "-1 1 max"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::top());
  "MIN-INT 0 max"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::top());
  "MIN-INT MAX-INT max"_fs;
  TEST_ASSERT_EQUAL_INT(MAX_INT, shi::top());
  "0 MAX-INT max"_fs;
  TEST_ASSERT_EQUAL_INT(MAX_INT, shi::top());
  "0 0 max"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::top());
  "1 1 max"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::top());
  "1 0 max"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::top());
  "2 1 max"_fs;
  "0 -1 max"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::top());
  "1 -1 max"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::top());
  "0 MIN-INT max"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::top());
  "MAX-INT MIN-INT max"_fs;
  TEST_ASSERT_EQUAL_INT(MAX_INT, shi::top());
  "MAX-INT 0 max"_fs;
  TEST_ASSERT_EQUAL_INT(MAX_INT, shi::top());

  shi::clear();
}

TEST(_2drop__2dup__2over__2swap_q_dup__depth__drop__dup__over__rot__swap) {
  "1 2 2drop"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::size());

  "1 2 2dup"_fs;
  TEST_ASSERT_EQUAL_INT(4, shi::size());
  TEST_ASSERT_EQUAL_INT(2, shi::top());
  TEST_ASSERT_EQUAL_INT(1, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(2, shi::top(-2));
  TEST_ASSERT_EQUAL_INT(1, shi::top(-3));

  "1 2 3 4 2over"_fs;
  TEST_ASSERT_EQUAL_INT(10, shi::size());
  TEST_ASSERT_EQUAL_INT(2, shi::top());
  TEST_ASSERT_EQUAL_INT(1, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(4, shi::top(-2));
  TEST_ASSERT_EQUAL_INT(3, shi::top(-3));
  TEST_ASSERT_EQUAL_INT(2, shi::top(-4));
  TEST_ASSERT_EQUAL_INT(1, shi::top(-5));
  TEST_ASSERT_EQUAL_INT(2, shi::top(-6));
  TEST_ASSERT_EQUAL_INT(1, shi::top(-7));
  TEST_ASSERT_EQUAL_INT(2, shi::top(-8));
  TEST_ASSERT_EQUAL_INT(1, shi::top(-9));

  shi::clear();

  "1 2 3 4 2swap"_fs;
  TEST_ASSERT_EQUAL_INT(4, shi::size());
  TEST_ASSERT_EQUAL_INT(2, shi::top());
  TEST_ASSERT_EQUAL_INT(1, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(4, shi::top(-2));
  TEST_ASSERT_EQUAL_INT(3, shi::top(-3));

  "0 ?dup"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::top());

  "1 ?dup"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::top());
  TEST_ASSERT_EQUAL_INT(1, shi::top(-1));

  "-1 ?dup"_fs;
  TEST_ASSERT_EQUAL_INT(-1, shi::top());
  TEST_ASSERT_EQUAL_INT(-1, shi::top(-1));

  shi::clear();

  "depth"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::top());

  shi::clear();

  "0 depth"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::top());
  TEST_ASSERT_EQUAL_INT(0, shi::top(-1));

  shi::clear();

  "0 1 depth"_fs;
  TEST_ASSERT_EQUAL_INT(2, shi::top());
  TEST_ASSERT_EQUAL_INT(1, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(0, shi::top(-2));

  shi::clear();

  "0 drop"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::size());

  "1 2 drop"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::size());
  TEST_ASSERT_EQUAL_INT(1, shi::top());

  "1 dup"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::top());
  TEST_ASSERT_EQUAL_INT(1, shi::top(-1));

  "1 2 over"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::top());
  TEST_ASSERT_EQUAL_INT(2, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(1, shi::top(-2));

  "1 2 3 rot"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::top());
  TEST_ASSERT_EQUAL_INT(3, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(2, shi::top(-2));

  "1 2 swap"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::top());
  TEST_ASSERT_EQUAL_INT(2, shi::top(-1));

  shi::clear();
}

TEST(to_r__from_r__r_fetch) {
  ": GR1 >r r> ;"_fs;
  ": GR2 >r r@ r> drop ;"_fs;

  "123 GR1"_fs;
  TEST_ASSERT_EQUAL_INT(123, shi::top());

  "123 GR2"_fs;
  TEST_ASSERT_EQUAL_INT(123, shi::top());
  TEST_ASSERT_EQUAL_INT(123, shi::top(-1));

  "1S GR1"_fs;
  TEST_ASSERT_EQUAL_INT(_1S, shi::top());

  shi::clear();
}

TEST(plus__minus__one_plus__one_minus__abs__negate) {
  "0 5 +"_fs;
  TEST_ASSERT_EQUAL_INT(5, shi::top());
  "5 0 +"_fs;
  TEST_ASSERT_EQUAL_INT(5, shi::top());
  "0 -5 +"_fs;
  TEST_ASSERT_EQUAL_INT(-5, shi::top());
  "-5 0 +"_fs;
  TEST_ASSERT_EQUAL_INT(-5, shi::top());
  "1 2 +"_fs;
  TEST_ASSERT_EQUAL_INT(3, shi::top());
  "1 -2 +"_fs;
  TEST_ASSERT_EQUAL_INT(-1, shi::top());
  "-1 2 +"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::top());
  "-1 -2 +"_fs;
  TEST_ASSERT_EQUAL_INT(-3, shi::top());
  "-1 1 +"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::top());
  "MID-UINT 1 +"_fs;
  TEST_ASSERT_EQUAL_INT(MID_UINT_p1, shi::top());

  "0 5 -"_fs;
  TEST_ASSERT_EQUAL_INT(-5, shi::top());
  "5 0 -"_fs;
  TEST_ASSERT_EQUAL_INT(5, shi::top());
  "0 -5 -"_fs;
  TEST_ASSERT_EQUAL_INT(5, shi::top());
  "-5 0 -"_fs;
  TEST_ASSERT_EQUAL_INT(-5, shi::top());
  "1 2 -"_fs;
  TEST_ASSERT_EQUAL_INT(-1, shi::top());
  "1 -2 -"_fs;
  TEST_ASSERT_EQUAL_INT(3, shi::top());
  "-1 2 -"_fs;
  TEST_ASSERT_EQUAL_INT(-3, shi::top());
  "-1 -2 -"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::top());
  "0 1 -"_fs;
  TEST_ASSERT_EQUAL_INT(-1, shi::top());
  "MID-UINT+1 1 -"_fs;
  TEST_ASSERT_EQUAL_INT(MID_UINT, shi::top());

  "0 1+"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::top());
  "-1 1+"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::top());
  "1 1+"_fs;
  TEST_ASSERT_EQUAL_INT(2, shi::top());
  "MID-UINT 1+"_fs;
  TEST_ASSERT_EQUAL_INT(MID_UINT_p1, shi::top());

  "2 1-"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::top());
  "1 1-"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::top());
  "0 1-"_fs;
  TEST_ASSERT_EQUAL_INT(-1, shi::top());
  "MID-UINT+1 1-"_fs;
  TEST_ASSERT_EQUAL_INT(MID_UINT, shi::top());

  "0 negate"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::top());
  "1 negate"_fs;
  TEST_ASSERT_EQUAL_INT(-1, shi::top());
  "-1 negate"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::top());
  "2 negate"_fs;
  TEST_ASSERT_EQUAL_INT(-2, shi::top());
  "-2 negate"_fs;
  TEST_ASSERT_EQUAL_INT(2, shi::top());

  "0 abs"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::top());
  "1 abs"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::top());
  "-1 abs"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::top());
  "MIN-INT abs"_fs;
  TEST_ASSERT_EQUAL_INT(MID_UINT_p1, shi::top());

  shi::clear();
}

TEST(s_to_d__m_times__um_times) {
  //  T{ 0 S>D -> 0 0 }T
  //  T{ 1 S>D -> 1 0 }T
  //  T{ 2 S>D -> 2 0 }T
  //  T{ -1 S>D -> -1 -1 }T
  //  T{ -2 S>D -> -2 -1 }T
  //  T{ MIN-INT S>D -> MIN-INT -1 }T
  //  T{ MAX-INT S>D -> MAX-INT 0 }T
  //
  //  T{ 0 0 M* -> 0 S>D }T
  //  T{ 0 1 M* -> 0 S>D }T
  //  T{ 1 0 M* -> 0 S>D }T
  //  T{ 1 2 M* -> 2 S>D }T
  //  T{ 2 1 M* -> 2 S>D }T
  //  T{ 3 3 M* -> 9 S>D }T
  //  T{ -3 3 M* -> -9 S>D }T
  //  T{ 3 -3 M* -> -9 S>D }T
  //  T{ -3 -3 M* -> 9 S>D }T
  //  T{ 0 MIN-INT M* -> 0 S>D }T
  //  T{ 1 MIN-INT M* -> MIN-INT S>D }T
  //  T{ 2 MIN-INT M* -> 0 1S }T
  //  T{ 0 MAX-INT M* -> 0 S>D }T
  //  T{ 1 MAX-INT M* -> MAX-INT S>D }T
  //  T{ 2 MAX-INT M* -> MAX-INT 1 LSHIFT 0 }T
  //  T{ MIN-INT MIN-INT M* -> 0 MSB 1 RSHIFT }T
  //  T{ MAX-INT MIN-INT M* -> MSB MSB 2/ }T
  //  T{ MAX-INT MAX-INT M* -> 1 MSB 2/ INVERT }T
  //
  //  T{ 0 0 * -> 0 }T            \ TEST IDENTITIES
  //  T{ 0 1 * -> 0 }T
  //  T{ 1 0 * -> 0 }T
  //  T{ 1 2 * -> 2 }T
  //  T{ 2 1 * -> 2 }T
  //  T{ 3 3 * -> 9 }T
  //  T{ -3 3 * -> -9 }T
  //  T{ 3 -3 * -> -9 }T
  //  T{ -3 -3 * -> 9 }T
  //
  //  T{ MID-UINT+1 1 RSHIFT 2 * -> MID-UINT+1 }T
  //  T{ MID-UINT+1 2 RSHIFT 4 * -> MID-UINT+1 }T
  //  T{ MID-UINT+1 1 RSHIFT MID-UINT+1 OR 2 * -> MID-UINT+1 }T
  //
  //  T{ 0 0 UM* -> 0 0 }T
  //  T{ 0 1 UM* -> 0 0 }T
  //  T{ 1 0 UM* -> 0 0 }T
  //  T{ 1 2 UM* -> 2 0 }T
  //  T{ 2 1 UM* -> 2 0 }T
  //  T{ 3 3 UM* -> 9 0 }T
  //
  //  T{ MID-UINT+1 1 RSHIFT 2 UM* -> MID-UINT+1 0 }T
  //  T{ MID-UINT+1 2 UM* -> 0 1 }T
  //  T{ MID-UINT+1 4 UM* -> 0 2 }T
  //  T{ 1S 2 UM* -> 1S 1 LSHIFT 1 }T
  //  T{ MAX-UINT MAX-UINT UM* -> 1 1 INVERT }T
}

TEST(
    fm_div_mod__sm_div_rem__um_div_mod__times_div__times_div_mod__div__div_mod__mod) {
  //  T{ 0 S>D 1 FM/MOD -> 0 0 }T
  //  T{ 1 S>D 1 FM/MOD -> 0 1 }T
  //  T{ 2 S>D 1 FM/MOD -> 0 2 }T
  //  T{ -1 S>D 1 FM/MOD -> 0 -1 }T
  //  T{ -2 S>D 1 FM/MOD -> 0 -2 }T
  //  T{ 0 S>D -1 FM/MOD -> 0 0 }T
  //  T{ 1 S>D -1 FM/MOD -> 0 -1 }T
  //  T{ 2 S>D -1 FM/MOD -> 0 -2 }T
  //  T{ -1 S>D -1 FM/MOD -> 0 1 }T
  //  T{ -2 S>D -1 FM/MOD -> 0 2 }T
  //  T{ 2 S>D 2 FM/MOD -> 0 1 }T
  //  T{ -1 S>D -1 FM/MOD -> 0 1 }T
  //  T{ -2 S>D -2 FM/MOD -> 0 1 }T
  //  T{  7 S>D  3 FM/MOD -> 1 2 }T
  //  T{  7 S>D -3 FM/MOD -> -2 -3 }T
  //  T{ -7 S>D  3 FM/MOD -> 2 -3 }T
  //  T{ -7 S>D -3 FM/MOD -> -1 2 }T
  //  T{ MAX-INT S>D 1 FM/MOD -> 0 MAX-INT }T
  //  T{ MIN-INT S>D 1 FM/MOD -> 0 MIN-INT }T
  //  T{ MAX-INT S>D MAX-INT FM/MOD -> 0 1 }T
  //  T{ MIN-INT S>D MIN-INT FM/MOD -> 0 1 }T
  //  T{ 1S 1 4 FM/MOD -> 3 MAX-INT }T
  //  T{ 1 MIN-INT M* 1 FM/MOD -> 0 MIN-INT }T
  //  T{ 1 MIN-INT M* MIN-INT FM/MOD -> 0 1 }T
  //  T{ 2 MIN-INT M* 2 FM/MOD -> 0 MIN-INT }T
  //  T{ 2 MIN-INT M* MIN-INT FM/MOD -> 0 2 }T
  //  T{ 1 MAX-INT M* 1 FM/MOD -> 0 MAX-INT }T
  //  T{ 1 MAX-INT M* MAX-INT FM/MOD -> 0 1 }T
  //  T{ 2 MAX-INT M* 2 FM/MOD -> 0 MAX-INT }T
  //  T{ 2 MAX-INT M* MAX-INT FM/MOD -> 0 2 }T
  //  T{ MIN-INT MIN-INT M* MIN-INT FM/MOD -> 0 MIN-INT }T
  //  T{ MIN-INT MAX-INT M* MIN-INT FM/MOD -> 0 MAX-INT }T
  //  T{ MIN-INT MAX-INT M* MAX-INT FM/MOD -> 0 MIN-INT }T
  //  T{ MAX-INT MAX-INT M* MAX-INT FM/MOD -> 0 MAX-INT }T
  //
  //  T{ 0 S>D 1 SM/REM -> 0 0 }T
  //  T{ 1 S>D 1 SM/REM -> 0 1 }T
  //  T{ 2 S>D 1 SM/REM -> 0 2 }T
  //  T{ -1 S>D 1 SM/REM -> 0 -1 }T
  //  T{ -2 S>D 1 SM/REM -> 0 -2 }T
  //  T{ 0 S>D -1 SM/REM -> 0 0 }T
  //  T{ 1 S>D -1 SM/REM -> 0 -1 }T
  //  T{ 2 S>D -1 SM/REM -> 0 -2 }T
  //  T{ -1 S>D -1 SM/REM -> 0 1 }T
  //  T{ -2 S>D -1 SM/REM -> 0 2 }T
  //  T{ 2 S>D 2 SM/REM -> 0 1 }T
  //  T{ -1 S>D -1 SM/REM -> 0 1 }T
  //  T{ -2 S>D -2 SM/REM -> 0 1 }T
  //  T{  7 S>D  3 SM/REM -> 1 2 }T
  //  T{  7 S>D -3 SM/REM -> 1 -2 }T
  //  T{ -7 S>D  3 SM/REM -> -1 -2 }T
  //  T{ -7 S>D -3 SM/REM -> -1 2 }T
  //  T{ MAX-INT S>D 1 SM/REM -> 0 MAX-INT }T
  //  T{ MIN-INT S>D 1 SM/REM -> 0 MIN-INT }T
  //  T{ MAX-INT S>D MAX-INT SM/REM -> 0 1 }T
  //  T{ MIN-INT S>D MIN-INT SM/REM -> 0 1 }T
  //  T{ 1S 1 4 SM/REM -> 3 MAX-INT }T
  //  T{ 2 MIN-INT M* 2 SM/REM -> 0 MIN-INT }T
  //  T{ 2 MIN-INT M* MIN-INT SM/REM -> 0 2 }T
  //  T{ 2 MAX-INT M* 2 SM/REM -> 0 MAX-INT }T
  //  T{ 2 MAX-INT M* MAX-INT SM/REM -> 0 2 }T
  //  T{ MIN-INT MIN-INT M* MIN-INT SM/REM -> 0 MIN-INT }T
  //  T{ MIN-INT MAX-INT M* MIN-INT SM/REM -> 0 MAX-INT }T
  //  T{ MIN-INT MAX-INT M* MAX-INT SM/REM -> 0 MIN-INT }T
  //  T{ MAX-INT MAX-INT M* MAX-INT SM/REM -> 0 MAX-INT }T
  //
  //  T{ 0 0 1 UM/MOD -> 0 0 }T
  //  T{ 1 0 1 UM/MOD -> 0 1 }T
  //  T{ 1 0 2 UM/MOD -> 1 0 }T
  //  T{ 3 0 2 UM/MOD -> 1 1 }T
  //  T{ MAX-UINT 2 UM* 2 UM/MOD -> 0 MAX-UINT }T
  //  T{ MAX-UINT 2 UM* MAX-UINT UM/MOD -> 0 2 }T
  //  T{ MAX-UINT MAX-UINT UM* MAX-UINT UM/MOD -> 0 MAX-UINT }T
  //
  //  : IFFLOORED
  //     [ -3 2 / -2 = INVERT ] LITERAL IF POSTPONE \ THEN ;
  //
  //  : IFSYM
  //     [ -3 2 / -1 = INVERT ] LITERAL IF POSTPONE \ THEN ;
  //
  //  \ THE SYSTEM MIGHT DO EITHER FLOORED OR SYMMETRIC DIVISION.
  //  \ SINCE WE HAVE ALREADY TESTED M*, FM/MOD, AND SM/REM WE CAN USE THEM IN
  //  TEST.
  //
  //  IFFLOORED : T/MOD  >R S>D R> FM/MOD ;
  //  IFFLOORED : T/     T/MOD SWAP DROP ;
  //  IFFLOORED : TMOD   T/MOD DROP ;
  //  IFFLOORED : T*/MOD >R M* R> FM/MOD ;
  //  IFFLOORED : T*/    T*/MOD SWAP DROP ;
  //  IFSYM     : T/MOD  >R S>D R> SM/REM ;
  //  IFSYM     : T/     T/MOD SWAP DROP ;
  //  IFSYM     : TMOD   T/MOD DROP ;
  //  IFSYM     : T*/MOD >R M* R> SM/REM ;
  //  IFSYM     : T*/    T*/MOD SWAP DROP ;
  //
  //  T{ 0 1 /MOD -> 0 1 T/MOD }T
  //  T{ 1 1 /MOD -> 1 1 T/MOD }T
  //  T{ 2 1 /MOD -> 2 1 T/MOD }T
  //  T{ -1 1 /MOD -> -1 1 T/MOD }T
  //  T{ -2 1 /MOD -> -2 1 T/MOD }T
  //  T{ 0 -1 /MOD -> 0 -1 T/MOD }T
  //  T{ 1 -1 /MOD -> 1 -1 T/MOD }T
  //  T{ 2 -1 /MOD -> 2 -1 T/MOD }T
  //  T{ -1 -1 /MOD -> -1 -1 T/MOD }T
  //  T{ -2 -1 /MOD -> -2 -1 T/MOD }T
  //  T{ 2 2 /MOD -> 2 2 T/MOD }T
  //  T{ -1 -1 /MOD -> -1 -1 T/MOD }T
  //  T{ -2 -2 /MOD -> -2 -2 T/MOD }T
  //  T{ 7 3 /MOD -> 7 3 T/MOD }T
  //  T{ 7 -3 /MOD -> 7 -3 T/MOD }T
  //  T{ -7 3 /MOD -> -7 3 T/MOD }T
  //  T{ -7 -3 /MOD -> -7 -3 T/MOD }T
  //  T{ MAX-INT 1 /MOD -> MAX-INT 1 T/MOD }T
  //  T{ MIN-INT 1 /MOD -> MIN-INT 1 T/MOD }T
  //  T{ MAX-INT MAX-INT /MOD -> MAX-INT MAX-INT T/MOD }T
  //  T{ MIN-INT MIN-INT /MOD -> MIN-INT MIN-INT T/MOD }T
  //
  //  T{ 0 1 / -> 0 1 T/ }T
  //  T{ 1 1 / -> 1 1 T/ }T
  //  T{ 2 1 / -> 2 1 T/ }T
  //  T{ -1 1 / -> -1 1 T/ }T
  //  T{ -2 1 / -> -2 1 T/ }T
  //  T{ 0 -1 / -> 0 -1 T/ }T
  //  T{ 1 -1 / -> 1 -1 T/ }T
  //  T{ 2 -1 / -> 2 -1 T/ }T
  //  T{ -1 -1 / -> -1 -1 T/ }T
  //  T{ -2 -1 / -> -2 -1 T/ }T
  //  T{ 2 2 / -> 2 2 T/ }T
  //  T{ -1 -1 / -> -1 -1 T/ }T
  //  T{ -2 -2 / -> -2 -2 T/ }T
  //  T{ 7 3 / -> 7 3 T/ }T
  //  T{ 7 -3 / -> 7 -3 T/ }T
  //  T{ -7 3 / -> -7 3 T/ }T
  //  T{ -7 -3 / -> -7 -3 T/ }T
  //  T{ MAX-INT 1 / -> MAX-INT 1 T/ }T
  //  T{ MIN-INT 1 / -> MIN-INT 1 T/ }T
  //  T{ MAX-INT MAX-INT / -> MAX-INT MAX-INT T/ }T
  //  T{ MIN-INT MIN-INT / -> MIN-INT MIN-INT T/ }T
  //
  //  T{ 0 1 MOD -> 0 1 TMOD }T
  //  T{ 1 1 MOD -> 1 1 TMOD }T
  //  T{ 2 1 MOD -> 2 1 TMOD }T
  //  T{ -1 1 MOD -> -1 1 TMOD }T
  //  T{ -2 1 MOD -> -2 1 TMOD }T
  //  T{ 0 -1 MOD -> 0 -1 TMOD }T
  //  T{ 1 -1 MOD -> 1 -1 TMOD }T
  //  T{ 2 -1 MOD -> 2 -1 TMOD }T
  //  T{ -1 -1 MOD -> -1 -1 TMOD }T
  //  T{ -2 -1 MOD -> -2 -1 TMOD }T
  //  T{ 2 2 MOD -> 2 2 TMOD }T
  //  T{ -1 -1 MOD -> -1 -1 TMOD }T
  //  T{ -2 -2 MOD -> -2 -2 TMOD }T
  //  T{ 7 3 MOD -> 7 3 TMOD }T
  //  T{ 7 -3 MOD -> 7 -3 TMOD }T
  //  T{ -7 3 MOD -> -7 3 TMOD }T
  //  T{ -7 -3 MOD -> -7 -3 TMOD }T
  //  T{ MAX-INT 1 MOD -> MAX-INT 1 TMOD }T
  //  T{ MIN-INT 1 MOD -> MIN-INT 1 TMOD }T
  //  T{ MAX-INT MAX-INT MOD -> MAX-INT MAX-INT TMOD }T
  //  T{ MIN-INT MIN-INT MOD -> MIN-INT MIN-INT TMOD }T
  //
  //  T{ 0 2 1 */ -> 0 2 1 T*/ }T
  //  T{ 1 2 1 */ -> 1 2 1 T*/ }T
  //  T{ 2 2 1 */ -> 2 2 1 T*/ }T
  //  T{ -1 2 1 */ -> -1 2 1 T*/ }T
  //  T{ -2 2 1 */ -> -2 2 1 T*/ }T
  //  T{ 0 2 -1 */ -> 0 2 -1 T*/ }T
  //  T{ 1 2 -1 */ -> 1 2 -1 T*/ }T
  //  T{ 2 2 -1 */ -> 2 2 -1 T*/ }T
  //  T{ -1 2 -1 */ -> -1 2 -1 T*/ }T
  //  T{ -2 2 -1 */ -> -2 2 -1 T*/ }T
  //  T{ 2 2 2 */ -> 2 2 2 T*/ }T
  //  T{ -1 2 -1 */ -> -1 2 -1 T*/ }T
  //  T{ -2 2 -2 */ -> -2 2 -2 T*/ }T
  //  T{ 7 2 3 */ -> 7 2 3 T*/ }T
  //  T{ 7 2 -3 */ -> 7 2 -3 T*/ }T
  //  T{ -7 2 3 */ -> -7 2 3 T*/ }T
  //  T{ -7 2 -3 */ -> -7 2 -3 T*/ }T
  //  T{ MAX-INT 2 MAX-INT */ -> MAX-INT 2 MAX-INT T*/ }T
  //  T{ MIN-INT 2 MIN-INT */ -> MIN-INT 2 MIN-INT T*/ }T
  //
  //  T{ 0 2 1 */MOD -> 0 2 1 T*/MOD }T
  //  T{ 1 2 1 */MOD -> 1 2 1 T*/MOD }T
  //  T{ 2 2 1 */MOD -> 2 2 1 T*/MOD }T
  //  T{ -1 2 1 */MOD -> -1 2 1 T*/MOD }T
  //  T{ -2 2 1 */MOD -> -2 2 1 T*/MOD }T
  //  T{ 0 2 -1 */MOD -> 0 2 -1 T*/MOD }T
  //  T{ 1 2 -1 */MOD -> 1 2 -1 T*/MOD }T
  //  T{ 2 2 -1 */MOD -> 2 2 -1 T*/MOD }T
  //  T{ -1 2 -1 */MOD -> -1 2 -1 T*/MOD }T
  //  T{ -2 2 -1 */MOD -> -2 2 -1 T*/MOD }T
  //  T{ 2 2 2 */MOD -> 2 2 2 T*/MOD }T
  //  T{ -1 2 -1 */MOD -> -1 2 -1 T*/MOD }T
  //  T{ -2 2 -2 */MOD -> -2 2 -2 T*/MOD }T
  //  T{ 7 2 3 */MOD -> 7 2 3 T*/MOD }T
  //  T{ 7 2 -3 */MOD -> 7 2 -3 T*/MOD }T
  //  T{ -7 2 3 */MOD -> -7 2 3 T*/MOD }T
  //  T{ -7 2 -3 */MOD -> -7 2 -3 T*/MOD }T
  //  T{ MAX-INT 2 MAX-INT */MOD -> MAX-INT 2 MAX-INT T*/MOD }T
  //  T{ MIN-INT 2 MIN-INT */MOD -> MIN-INT 2 MIN-INT T*/MOD }T
}

TEST(
    comma__fetch__store__cell_plus__cells__c_comma__c_fetch__c_store__chars__two_fetch__two_store__align__aligned__plus_store__allot) {

}

TEST(unused0) {
  ",toram"_fs;
  "unused"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::size());
  auto top = shi::top();
  TEST_ASSERT_EQUAL_INT(SHI_RAM_END - SHI_RAM_START, shi::top());

  ",toflash"_fs;
  "unused"_fs;
  TEST_ASSERT_EQUAL_INT(2, shi::size());
  top = shi::top();
  TEST_ASSERT_EQUAL_INT(SHI_FLASH_END - SHI_FLASH_START, shi::top());

  ",toram"_fs;

  shi::clear();
}

TEST(unused1) {
  ",toram"_fs;
  "unused"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::size());
  TEST_ASSERT_LESS_THAN_INT(SHI_RAM_END - SHI_RAM_START, shi::top());

  shi::clear();
}

TEST(numeric_notation) {
  "#1289"_fs;
  TEST_ASSERT_EQUAL_INT(1289, shi::top());

  "#12346789"_fs;
  TEST_ASSERT_EQUAL_INT(12346789, shi::top());

  "#-1289"_fs;
  TEST_ASSERT_EQUAL_INT(-1289, shi::top());

  "$12EF"_fs;  // Upper case hex only!
  TEST_ASSERT_EQUAL_INT(0x12eF, shi::top());

  "$-12EF"_fs;
  TEST_ASSERT_EQUAL_INT(-0x12eF, shi::top());

  "%10010110"_fs;
  TEST_ASSERT_EQUAL_INT(0b10010110, shi::top());

  "%-10010110"_fs;
  TEST_ASSERT_EQUAL_INT(-0b10010110, shi::top());

  shi::clear();
}

TEST(top) {
  "13"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::size());
  TEST_ASSERT_EQUAL_INT(13, shi::top());

  "170"_fs;
  TEST_ASSERT_EQUAL_INT(2, shi::size());
  TEST_ASSERT_EQUAL_INT(170, shi::top());
  TEST_ASSERT_EQUAL_INT(13, shi::top(1));
  TEST_ASSERT_EQUAL_INT(13, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(2, shi::size());

  "38"_fs;
  TEST_ASSERT_EQUAL_INT(3, shi::size());
  TEST_ASSERT_EQUAL_INT(38, shi::top());
  TEST_ASSERT_EQUAL_INT(170, shi::top(1));
  TEST_ASSERT_EQUAL_INT(170, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(13, shi::top(2));
  TEST_ASSERT_EQUAL_INT(13, shi::top(-2));

  shi::clear();
}

TEST(if_else_then) {
  ": gi2 if 123 else 234 then ;"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::size());
  TEST_ASSERT_TRUE(shi::empty());

  "-1 gi2"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::size());
  TEST_ASSERT_EQUAL_INT(123, shi::top());

  "1 gi2"_fs;
  TEST_ASSERT_EQUAL_INT(2, shi::size());
  TEST_ASSERT_EQUAL_INT(123, shi::top());
  TEST_ASSERT_EQUAL_INT(123, shi::top(-1));

  "0 gi2"_fs;
  TEST_ASSERT_EQUAL_INT(3, shi::size());
  TEST_ASSERT_EQUAL_INT(234, shi::top());
  TEST_ASSERT_EQUAL_INT(123, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(123, shi::top(-2));

  ": melse if 1 else 2 else 3 else 4 else 5 then ;"_fs;

  "false melse"_fs;
  TEST_ASSERT_EQUAL_INT(5, shi::size());
  TEST_ASSERT_EQUAL_INT(4, shi::top());
  TEST_ASSERT_EQUAL_INT(2, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(234, shi::top(-2));
  TEST_ASSERT_EQUAL_INT(123, shi::top(-3));
  TEST_ASSERT_EQUAL_INT(123, shi::top(-4));

  "true melse"_fs;
  TEST_ASSERT_EQUAL_INT(8, shi::size());
  TEST_ASSERT_EQUAL_INT(5, shi::top());
  TEST_ASSERT_EQUAL_INT(3, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(1, shi::top(-2));
  TEST_ASSERT_EQUAL_INT(4, shi::top(-3));
  TEST_ASSERT_EQUAL_INT(2, shi::top(-4));
  TEST_ASSERT_EQUAL_INT(234, shi::top(-5));
  TEST_ASSERT_EQUAL_INT(123, shi::top(-6));
  TEST_ASSERT_EQUAL_INT(123, shi::top(-7));

  shi::clear();
}

TEST(loop) {
  ": gd1 do i loop ;"_fs;
  TEST_ASSERT_TRUE(shi::empty());

  "4 1 gd1"_fs;
  TEST_ASSERT_EQUAL_INT(3, shi::size());
  TEST_ASSERT_EQUAL_INT(3, shi::top());
  TEST_ASSERT_EQUAL_INT(2, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(1, shi::top(-2));

  "2 -1 gd1"_fs;
  TEST_ASSERT_EQUAL_INT(6, shi::size());
  TEST_ASSERT_EQUAL_INT(1, shi::top(0));
  TEST_ASSERT_EQUAL_INT(0, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(-1, shi::top(-2));
  TEST_ASSERT_EQUAL_INT(3, shi::top(-3));
  TEST_ASSERT_EQUAL_INT(2, shi::top(-4));
  TEST_ASSERT_EQUAL_INT(1, shi::top(-5));

  shi::clear();
}

TEST(leave) {
  ": gd5 123 swap 0 do i 4 > if drop 234 leave then loop ;"_fs;
  TEST_ASSERT_TRUE(shi::empty());

  "1 gd5"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::size());
  TEST_ASSERT_EQUAL_INT(123, shi::top());

  "5 gd5"_fs;
  TEST_ASSERT_EQUAL_INT(2, shi::size());
  TEST_ASSERT_EQUAL_INT(123, shi::top());
  TEST_ASSERT_EQUAL_INT(123, shi::top(-1));

  "6 gd5"_fs;
  TEST_ASSERT_EQUAL_INT(3, shi::size());
  TEST_ASSERT_EQUAL_INT(234, shi::top());
  TEST_ASSERT_EQUAL_INT(123, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(123, shi::top(-2));

  shi::clear();
}

TEST(case_) {
  ": cs1 case 1 of 111 endof 2 of 222 endof 3 of 333 endof >r 999 r> endcase ;"_fs;
  TEST_ASSERT_TRUE(shi::empty());

  "1 cs1"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::size());
  TEST_ASSERT_EQUAL_INT(111, shi::top());

  "2 cs1"_fs;
  TEST_ASSERT_EQUAL_INT(2, shi::size());
  TEST_ASSERT_EQUAL_INT(222, shi::top());
  TEST_ASSERT_EQUAL_INT(111, shi::top(-1));

  "3 cs1"_fs;
  TEST_ASSERT_EQUAL_INT(3, shi::size());
  TEST_ASSERT_EQUAL_INT(333, shi::top());
  TEST_ASSERT_EQUAL_INT(222, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(111, shi::top(-2));

  "4 cs1"_fs;
  TEST_ASSERT_EQUAL_INT(4, shi::size());
  TEST_ASSERT_EQUAL_INT(999, shi::top());
  TEST_ASSERT_EQUAL_INT(333, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(222, shi::top(-2));
  TEST_ASSERT_EQUAL_INT(111, shi::top(-3));

  ": cs2 >r case -1 of case r@ 1 of 100 endof 2 of 200 endof >r -300 r> endcase endof -2 of case r@ 1 of -99 endof >r -199 r> endcase endof >r 299 r> endcase r> drop ;"_fs;
  TEST_ASSERT_EQUAL_INT(4, shi::size());

  "-1 1 cs2"_fs;
  TEST_ASSERT_EQUAL_INT(5, shi::size());
  TEST_ASSERT_EQUAL_INT(100, shi::top());
  TEST_ASSERT_EQUAL_INT(999, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(333, shi::top(-2));
  TEST_ASSERT_EQUAL_INT(222, shi::top(-3));
  TEST_ASSERT_EQUAL_INT(111, shi::top(-4));

  "-1 2 cs2"_fs;
  TEST_ASSERT_EQUAL_INT(6, shi::size());
  TEST_ASSERT_EQUAL_INT(200, shi::top());
  TEST_ASSERT_EQUAL_INT(100, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(999, shi::top(-2));
  TEST_ASSERT_EQUAL_INT(333, shi::top(-3));
  TEST_ASSERT_EQUAL_INT(222, shi::top(-4));
  TEST_ASSERT_EQUAL_INT(111, shi::top(-5));

  "-1 3 cs2"_fs;
  TEST_ASSERT_EQUAL_INT(7, shi::size());
  TEST_ASSERT_EQUAL_INT(-300, shi::top());
  TEST_ASSERT_EQUAL_INT(200, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(100, shi::top(-2));
  TEST_ASSERT_EQUAL_INT(999, shi::top(-3));
  TEST_ASSERT_EQUAL_INT(333, shi::top(-4));
  TEST_ASSERT_EQUAL_INT(222, shi::top(-5));
  TEST_ASSERT_EQUAL_INT(111, shi::top(-6));

  "-2 1 cs2"_fs;
  TEST_ASSERT_EQUAL_INT(8, shi::size());
  TEST_ASSERT_EQUAL_INT(-99, shi::top());
  TEST_ASSERT_EQUAL_INT(-300, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(200, shi::top(-2));
  TEST_ASSERT_EQUAL_INT(100, shi::top(-3));
  TEST_ASSERT_EQUAL_INT(999, shi::top(-4));
  TEST_ASSERT_EQUAL_INT(333, shi::top(-5));
  TEST_ASSERT_EQUAL_INT(222, shi::top(-6));
  TEST_ASSERT_EQUAL_INT(111, shi::top(-7));

  "-2 2 cs2"_fs;
  TEST_ASSERT_EQUAL_INT(9, shi::size());
  TEST_ASSERT_EQUAL_INT(-199, shi::top());
  TEST_ASSERT_EQUAL_INT(-99, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(-300, shi::top(-2));
  TEST_ASSERT_EQUAL_INT(200, shi::top(-3));
  TEST_ASSERT_EQUAL_INT(100, shi::top(-4));
  TEST_ASSERT_EQUAL_INT(999, shi::top(-5));
  TEST_ASSERT_EQUAL_INT(333, shi::top(-6));
  TEST_ASSERT_EQUAL_INT(222, shi::top(-7));
  TEST_ASSERT_EQUAL_INT(111, shi::top(-8));

  "0 2 cs2"_fs;
  TEST_ASSERT_EQUAL_INT(10, shi::size());
  TEST_ASSERT_EQUAL_INT(299, shi::top());
  TEST_ASSERT_EQUAL_INT(-199, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(-99, shi::top(-2));
  TEST_ASSERT_EQUAL_INT(-300, shi::top(-3));
  TEST_ASSERT_EQUAL_INT(200, shi::top(-4));
  TEST_ASSERT_EQUAL_INT(100, shi::top(-5));
  TEST_ASSERT_EQUAL_INT(999, shi::top(-6));
  TEST_ASSERT_EQUAL_INT(333, shi::top(-7));
  TEST_ASSERT_EQUAL_INT(222, shi::top(-8));
  TEST_ASSERT_EQUAL_INT(111, shi::top(-9));

  shi::clear();
}

TEST(variable) {
  "variable v1"_fs;
  TEST_ASSERT_TRUE(shi::empty());

  "123 v1 !"_fs;
  TEST_ASSERT_TRUE(shi::empty());

  "v1 @"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::size());
  TEST_ASSERT_EQUAL_INT(123, shi::top());

  shi::clear();
}

TEST(constant) {
  "123 constant x123"_fs;
  TEST_ASSERT_TRUE(shi::empty());

  "x123"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::size());
  TEST_ASSERT_EQUAL_INT(123, shi::top());

  ": equ constant ;"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::size());

  "x123 equ y123"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::size());

  "y123"_fs;
  TEST_ASSERT_EQUAL_INT(2, shi::size());
  TEST_ASSERT_EQUAL_INT(123, shi::top());
  TEST_ASSERT_EQUAL_INT(123, shi::top(-1));

  shi::clear();
}

// This looks super promising for writing flash memory stuff @ QEMU...
// https://dangokyo.me/2018/03/27/qemu-internal-memory-region-address-space-and-qemu-io/#more-1697

int main() {
  // Reset of all peripherals, Initializes the Flash interface and the Systick
  HAL_Init();
  // Configure the system clock
  SystemClock_Config();

  asm volatile("nop");
  shi::init(SHI_RAM_START, SHI_RAM_END, SHI_FLASH_START, SHI_FLASH_END);

  //  semihosting_io();

  UNITY_BEGIN();

  RUN_TEST(unused0);  // Needs to run at the very top

  RUN_TEST(basic_assumptions);
  RUN_TEST(invert__and__or__xor);
  RUN_TEST(two_times__two_div__lshift__rshit);
  RUN_TEST(zero_equal__equal__zero_less__less__more__u_less__min__max);
  RUN_TEST(_2drop__2dup__2over__2swap_q_dup__depth__drop__dup__over__rot__swap);
  RUN_TEST(to_r__from_r__r_fetch);
  RUN_TEST(plus__minus__one_plus__one_minus__abs__negate);
  RUN_TEST(s_to_d__m_times__um_times);
  RUN_TEST(
      fm_div_mod__sm_div_rem__um_div_mod__times_div__times_div_mod__div__div_mod__mod);
  RUN_TEST(
      comma__fetch__store__cell_plus__cells__c_comma__c_fetch__c_store__chars__two_fetch__two_store__align__aligned__plus_store__allot);

  RUN_TEST(numeric_notation);
  //  RUN_TEST(top);
  //  RUN_TEST(if_else_then);
  //  RUN_TEST(loop);
  //  RUN_TEST(leave);
  //  RUN_TEST(case_);
  //  RUN_TEST(variable);
  //  RUN_TEST(constant);

  RUN_TEST(unused1);  // Can run anywhere but at the top

  return UNITY_END();
}

void shi_test_compile_to_flash() {
  asm volatile("nop");

  // Compile to FLASH
  //  shi::evaluate(",toflash");
  //  shi::evaluate(": p13 13 ;");
  //  shi::evaluate(": p1314 p13 14 ;");
  //  shi::stack_print(printf);  // Empty
  //  shi::evaluate("p1314");    // 13 14
  //  shi::stack_print(printf);
  //  shi::evaluate(",toram");

  asm volatile("nop");
}

void shi_test() {
  // Tick (returns xt of definition)
  shi::evaluate("' cs1");  // some ... ram address
  shi::stack_print(printf);
  shi::evaluate("' gagsi_undefined");  // should get error message
  shi::stack_print(printf);
  shi::clear();
  asm volatile("nop");

  // Checking if a definition exists is rather complicated if you don't want to
  // execute it... You've got to push source on the stack, parse the following
  // name of the definition an then execute find on it...
  //
  // This returns either
  // ( -- token-addr false ) if nothing was found or
  // ( -- xt flags )         if the definition was found
  asm volatile("nop");
  shi::evaluate("source parse cs1 find");
  shi::stack_print(printf);

  // In theory we can now create a new definition, based upon if cs1 exists or
  // not
  auto f_exists = shi::top();
  if (f_exists)
    printf("definition cs1 found\n");
  else
    printf("definition cs1 not found\n");
  shi::evaluate("2drop");
  shi::stack_print(printf);

  // Lets try that again with a function we haven't defined yet
  shi::evaluate("source parse foo find");
  shi::stack_print(printf);
  f_exists = shi::top();
  if (f_exists)
    printf("definition foo found\n");
  else
    printf("definition foo not found\n");
  shi::clear();
  asm volatile("nop");

  // C variable
  asm volatile("nop");
  volatile uint32_t stars = 42;
  // equal to push(&stars) and evaluate("c-variable stars");
  shi::c_variable("stars", &stars);
  shi::stack_print(printf);
  shi::evaluate("stars @");
  shi::stack_print(printf);
  shi::evaluate("1 + stars !");
  shi::stack_print(printf);
  printf("Forth look how stars (our c-variable) turned out: %d\n", stars);
  shi::clear();

  // Stack functions
  asm volatile("nop");
  volatile int32_t int32{};
  volatile float f32_1{};
  volatile float f32_2{};
  shi::push(10);  // Push integer
  shi::stack_print(printf);
  int32 = shi::top();  // Read integer
  shi::push(32.4f);    // Push float
  shi::stack_print(printf);
  //  f32_1 = shi::top<float>();  // Read float
  //  shi::top(f32_2);            // Read float another (safer) way
  shi::clear();

  asm volatile("nop");
}

/// Overwritten SysTick handler
extern "C" void __attribute__((section(".after_vectors"))) SysTick_Handler() {
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
}

/// This function is executed in case of error occurrence
/// \param  file  The file name as string
/// \param  line  The line in file as a number
void _Error_Handler(char* file, int line) {
  while (1) {
  }
}

/// System Clock Configuration
void SystemClock_Config() {
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  // Configure the main internal regulator output voltage
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  // Initializes the CPU, AHB and APB busses clocks
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    _Error_Handler(__FILE__, __LINE__);
  }

  // Initializes the CPU, AHB and APB busses clocks
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
    _Error_Handler(__FILE__, __LINE__);
  }

  // Configure the Systick interrupt time
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

  // Configure the Systick
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  // SysTick_IRQn interrupt configuration
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}
