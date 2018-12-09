#include "forth2012_test_suite.hpp"
#include "shi.h"

using shi::operator""_fs;

TEST(basic_assumptions) {
  ": BITSSET? if 0 0 else 0 then ;"_fs;

  "0 BITSSET?"_fs;
  TEST_ASSERT_EQUAL(1, shi::size());
  TEST_ASSERT_EQUAL(0, shi::top());

  "1 BITSSET?"_fs;
  TEST_ASSERT_EQUAL(3, shi::size());
  TEST_ASSERT_EQUAL(0, shi::top());
  TEST_ASSERT_EQUAL(0, shi::top(-1));
  TEST_ASSERT_EQUAL(0, shi::top(-2));

  "-1 BITSSET?"_fs;
  TEST_ASSERT_EQUAL(5, shi::size());
  TEST_ASSERT_EQUAL(0, shi::top());
  TEST_ASSERT_EQUAL(0, shi::top(-1));
  TEST_ASSERT_EQUAL(0, shi::top(-2));
  TEST_ASSERT_EQUAL(0, shi::top(-3));
  TEST_ASSERT_EQUAL(0, shi::top(-4));

  shi::clear();
}

TEST(invert__and__or__xor) {
  "0 0 and"_fs;
  TEST_ASSERT_EQUAL(0, shi::top());
  "0 1 and"_fs;
  TEST_ASSERT_EQUAL(0, shi::top());
  "1 0 and"_fs;
  TEST_ASSERT_EQUAL(0, shi::top());
  "1 1 and"_fs;
  TEST_ASSERT_EQUAL(1, shi::top());
  "0 invert 1 and"_fs;
  TEST_ASSERT_EQUAL(1, shi::top());
  "1 invert 1 and"_fs;
  TEST_ASSERT_EQUAL(0, shi::top());

  "0 constant 0S"_fs;
  "0 invert constant 1S"_fs;

  "0S invert"_fs;
  TEST_ASSERT_EQUAL(_1S, shi::top());
  "1S invert"_fs;
  TEST_ASSERT_EQUAL(_0S, shi::top());

  "0S 0S or"_fs;
  TEST_ASSERT_EQUAL(_0S, shi::top());
  "0S 1S or"_fs;
  TEST_ASSERT_EQUAL(_1S, shi::top());
  "1S 0S or"_fs;
  TEST_ASSERT_EQUAL(_1S, shi::top());
  "1S 1S or"_fs;
  TEST_ASSERT_EQUAL(_1S, shi::top());

  "0S 0S xor"_fs;
  TEST_ASSERT_EQUAL(_0S, shi::top());
  "0S 1S xor"_fs;
  TEST_ASSERT_EQUAL(_1S, shi::top());
  "1S 0S xor"_fs;
  TEST_ASSERT_EQUAL(_1S, shi::top());
  "1S 1S xor"_fs;
  TEST_ASSERT_EQUAL(_0S, shi::top());

  shi::clear();
}

TEST(two_times__two_div__lshift__rshit) {
  "1S 1 rshift invert constant MSB"_fs;
  "MSB BITSSET?"_fs;
  TEST_ASSERT_EQUAL(2, shi::size());
  TEST_ASSERT_EQUAL(0, shi::top());
  TEST_ASSERT_EQUAL(0, shi::top(-1));

  "0S 2*"_fs;
  TEST_ASSERT_EQUAL(_0S, shi::top());
  "1 2*"_fs;
  TEST_ASSERT_EQUAL(2, shi::top());
  "4000 2*"_fs;
  TEST_ASSERT_EQUAL(8000, shi::top());
  "1S 2* 1 xor"_fs;
  TEST_ASSERT_EQUAL(_1S, shi::top());
  "MSB 2*"_fs;
  TEST_ASSERT_EQUAL(_0S, shi::top());

  "0S 2/"_fs;
  TEST_ASSERT_EQUAL(_0S, shi::top());
  "1 2/"_fs;
  TEST_ASSERT_EQUAL(0, shi::top());
  "4000 2/"_fs;
  TEST_ASSERT_EQUAL(2000, shi::top());
  "1S 2/"_fs;
  TEST_ASSERT_EQUAL(_1S, shi::top());
  "1S 1 xor 2/"_fs;
  TEST_ASSERT_EQUAL(_1S, shi::top());
  "MSB 2/ MSB and"_fs;
  TEST_ASSERT_EQUAL(MSB, shi::top());

  "1 0 lshift"_fs;
  TEST_ASSERT_EQUAL(1, shi::top());
  "1 1 lshift"_fs;
  TEST_ASSERT_EQUAL(2, shi::top());
  "1 2 lshift"_fs;
  TEST_ASSERT_EQUAL(4, shi::top());
  //  shi::evaluate("1 F lshift");  // -> 8000 }T         \ BIGGEST GUARANTEED
  //  SHIFT TEST_ASSERT_EQUAL(8000, shi::top());
  "1S 1 lshift 1 xor"_fs;
  TEST_ASSERT_EQUAL(_1S, shi::top());
  "MSB 1 lshift"_fs;
  TEST_ASSERT_EQUAL(0, shi::top());

  "1 0 rshift"_fs;
  TEST_ASSERT_EQUAL(1, shi::top());
  "1 1 rshift"_fs;
  TEST_ASSERT_EQUAL(0, shi::top());
  "2 1 rshift"_fs;
  TEST_ASSERT_EQUAL(1, shi::top());
  "4 2 rshift"_fs;
  TEST_ASSERT_EQUAL(1, shi::top());
  // shi::evaluate("8000 F rshift");  // -> 1 }T         \ BIGGEST
  //  TEST_ASSERT_EQUAL(1, shi::top());
  "MSB 1 rshift MSB and"_fs;
  TEST_ASSERT_EQUAL(0, shi::top());
  "MSB 1 rshift 2*"_fs;
  TEST_ASSERT_EQUAL(MSB, shi::top());

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
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "1 0="_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "2 0="_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "-1 0="_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "MAX-UINT 0="_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "MIN-INT 0="_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "MAX-INT 0="_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());

  "0 0 ="_fs;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "1 1 ="_fs;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "-1 -1 ="_fs;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "1 0 ="_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "-1 0 ="_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "0 1 ="_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "0 -1 ="_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());

  "0 0<"_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "-1 0<"_fs;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "MIN-INT 0<"_fs;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "1 0<"_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "MAX-INT 0<"_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());

  "0 1 <"_fs;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "1 2 <"_fs;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "-1 0 <"_fs;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "-1 1 <"_fs;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "MIN-INT 0 <"_fs;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "MIN-INT MAX-INT <"_fs;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "0 MAX-INT <"_fs;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "0 0 <"_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "1 1 <"_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "1 0 <"_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "2 1 <"_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "0 -1 <"_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "1 -1 <"_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "0 MIN-INT <"_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "MAX-INT MIN-INT <"_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "MAX-INT 0 <"_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());

  "0 1 >"_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "1 2 >"_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "-1 0 >"_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "-1 1 >"_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "MIN-INT 0 >"_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "MIN-INT MAX-INT >"_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "0 MAX-INT >"_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "0 0 >"_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "1 1 >"_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "1 0 >"_fs;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "2 1 >"_fs;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "0 -1 >"_fs;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "1 -1 >"_fs;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "0 MIN-INT >"_fs;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "MAX-INT MIN-INT >"_fs;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "MAX-INT 0 >"_fs;
  TEST_ASSERT_EQUAL(TRUE, shi::top());

  shi::clear();

  "0 1 u<"_fs;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "1 2 u<"_fs;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "0 MID-UINT u<"_fs;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "0 MAX-UINT u<"_fs;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "MID-UINT MAX-UINT u<"_fs;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "0 0 u<"_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "1 1 u<"_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "1 0 u<"_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "2 1 u<"_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "MID-UINT 0 u<"_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "MAX-UINT 0 u<"_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "MAX-UINT MID-UINT u<"_fs;
  TEST_ASSERT_EQUAL(FALSE, shi::top());

  "0 1 min"_fs;
  TEST_ASSERT_EQUAL(0, shi::top());
  "1 2 min"_fs;
  TEST_ASSERT_EQUAL(1, shi::top());
  "-1 0 min"_fs;
  TEST_ASSERT_EQUAL(-1, shi::top());
  "-1 1 min"_fs;
  TEST_ASSERT_EQUAL(-1, shi::top());
  "MIN-INT 0 min"_fs;
  TEST_ASSERT_EQUAL(MIN_INT, shi::top());
  "MIN-INT MAX-INT min"_fs;
  TEST_ASSERT_EQUAL(MIN_INT, shi::top());
  "0 MAX-INT min"_fs;
  TEST_ASSERT_EQUAL(0, shi::top());
  "0 0 min"_fs;
  TEST_ASSERT_EQUAL(0, shi::top());
  "1 1 min"_fs;
  TEST_ASSERT_EQUAL(1, shi::top());
  "1 0 min"_fs;
  TEST_ASSERT_EQUAL(0, shi::top());
  "2 1 min"_fs;
  TEST_ASSERT_EQUAL(1, shi::top());
  "0 -1 min"_fs;
  TEST_ASSERT_EQUAL(-1, shi::top());
  "1 -1 min"_fs;
  TEST_ASSERT_EQUAL(-1, shi::top());
  "0 MIN-INT min"_fs;
  TEST_ASSERT_EQUAL(MIN_INT, shi::top());
  "MAX-INT MIN-INT min"_fs;
  TEST_ASSERT_EQUAL(MIN_INT, shi::top());
  "MAX-INT 0 min"_fs;
  TEST_ASSERT_EQUAL(0, shi::top());

  "0 1 max"_fs;
  TEST_ASSERT_EQUAL(1, shi::top());
  "1 2 max"_fs;
  "-1 0 max"_fs;
  TEST_ASSERT_EQUAL(0, shi::top());
  "-1 1 max"_fs;
  TEST_ASSERT_EQUAL(1, shi::top());
  "MIN-INT 0 max"_fs;
  TEST_ASSERT_EQUAL(0, shi::top());
  "MIN-INT MAX-INT max"_fs;
  TEST_ASSERT_EQUAL(MAX_INT, shi::top());
  "0 MAX-INT max"_fs;
  TEST_ASSERT_EQUAL(MAX_INT, shi::top());
  "0 0 max"_fs;
  TEST_ASSERT_EQUAL(0, shi::top());
  "1 1 max"_fs;
  TEST_ASSERT_EQUAL(1, shi::top());
  "1 0 max"_fs;
  TEST_ASSERT_EQUAL(1, shi::top());
  "2 1 max"_fs;
  "0 -1 max"_fs;
  TEST_ASSERT_EQUAL(0, shi::top());
  "1 -1 max"_fs;
  TEST_ASSERT_EQUAL(1, shi::top());
  "0 MIN-INT max"_fs;
  TEST_ASSERT_EQUAL(0, shi::top());
  "MAX-INT MIN-INT max"_fs;
  TEST_ASSERT_EQUAL(MAX_INT, shi::top());
  "MAX-INT 0 max"_fs;
  TEST_ASSERT_EQUAL(MAX_INT, shi::top());

  shi::clear();
}

TEST(_2drop__2dup__2over__2swap_q_dup__depth__drop__dup__over__rot__swap) {
  "1 2 2drop"_fs;
  TEST_ASSERT_EQUAL(0, shi::size());

  "1 2 2dup"_fs;
  TEST_ASSERT_EQUAL(4, shi::size());
  TEST_ASSERT_EQUAL(2, shi::top());
  TEST_ASSERT_EQUAL(1, shi::top(-1));
  TEST_ASSERT_EQUAL(2, shi::top(-2));
  TEST_ASSERT_EQUAL(1, shi::top(-3));

  "1 2 3 4 2over"_fs;
  TEST_ASSERT_EQUAL(10, shi::size());
  TEST_ASSERT_EQUAL(2, shi::top());
  TEST_ASSERT_EQUAL(1, shi::top(-1));
  TEST_ASSERT_EQUAL(4, shi::top(-2));
  TEST_ASSERT_EQUAL(3, shi::top(-3));
  TEST_ASSERT_EQUAL(2, shi::top(-4));
  TEST_ASSERT_EQUAL(1, shi::top(-5));
  TEST_ASSERT_EQUAL(2, shi::top(-6));
  TEST_ASSERT_EQUAL(1, shi::top(-7));
  TEST_ASSERT_EQUAL(2, shi::top(-8));
  TEST_ASSERT_EQUAL(1, shi::top(-9));

  shi::clear();

  "1 2 3 4 2swap"_fs;
  TEST_ASSERT_EQUAL(4, shi::size());
  TEST_ASSERT_EQUAL(2, shi::top());
  TEST_ASSERT_EQUAL(1, shi::top(-1));
  TEST_ASSERT_EQUAL(4, shi::top(-2));
  TEST_ASSERT_EQUAL(3, shi::top(-3));

  "0 ?dup"_fs;
  TEST_ASSERT_EQUAL(0, shi::top());

  "1 ?dup"_fs;
  TEST_ASSERT_EQUAL(1, shi::top());
  TEST_ASSERT_EQUAL(1, shi::top(-1));

  "-1 ?dup"_fs;
  TEST_ASSERT_EQUAL(-1, shi::top());
  TEST_ASSERT_EQUAL(-1, shi::top(-1));

  shi::clear();

  "depth"_fs;
  TEST_ASSERT_EQUAL(0, shi::top());

  shi::clear();

  "0 depth"_fs;
  TEST_ASSERT_EQUAL(1, shi::top());
  TEST_ASSERT_EQUAL(0, shi::top(-1));

  shi::clear();

  "0 1 depth"_fs;
  TEST_ASSERT_EQUAL(2, shi::top());
  TEST_ASSERT_EQUAL(1, shi::top(-1));
  TEST_ASSERT_EQUAL(0, shi::top(-2));

  shi::clear();

  "0 drop"_fs;
  TEST_ASSERT_EQUAL(0, shi::size());

  "1 2 drop"_fs;
  TEST_ASSERT_EQUAL(1, shi::size());
  TEST_ASSERT_EQUAL(1, shi::top());

  "1 dup"_fs;
  TEST_ASSERT_EQUAL(1, shi::top());
  TEST_ASSERT_EQUAL(1, shi::top(-1));

  "1 2 over"_fs;
  TEST_ASSERT_EQUAL(1, shi::top());
  TEST_ASSERT_EQUAL(2, shi::top(-1));
  TEST_ASSERT_EQUAL(1, shi::top(-2));

  "1 2 3 rot"_fs;
  TEST_ASSERT_EQUAL(1, shi::top());
  TEST_ASSERT_EQUAL(3, shi::top(-1));
  TEST_ASSERT_EQUAL(2, shi::top(-2));

  "1 2 swap"_fs;
  TEST_ASSERT_EQUAL(1, shi::top());
  TEST_ASSERT_EQUAL(2, shi::top(-1));

  shi::clear();
}

TEST(to_r__from_r__r_fetch) {
  ": GR1 >r r> ;"_fs;
  ": GR2 >r r@ r> drop ;"_fs;

  "123 GR1"_fs;
  TEST_ASSERT_EQUAL(123, shi::top());

  "123 GR2"_fs;
  TEST_ASSERT_EQUAL(123, shi::top());
  TEST_ASSERT_EQUAL(123, shi::top(-1));

  "1S GR1"_fs;
  TEST_ASSERT_EQUAL(_1S, shi::top());

  shi::clear();
}

TEST(plus__minus__one_plus__one_minus__abs__negate) {
  "0 5 +"_fs;
  TEST_ASSERT_EQUAL(5, shi::top());
  "5 0 +"_fs;
  TEST_ASSERT_EQUAL(5, shi::top());
  "0 -5 +"_fs;
  TEST_ASSERT_EQUAL(-5, shi::top());
  "-5 0 +"_fs;
  TEST_ASSERT_EQUAL(-5, shi::top());
  "1 2 +"_fs;
  TEST_ASSERT_EQUAL(3, shi::top());
  "1 -2 +"_fs;
  TEST_ASSERT_EQUAL(-1, shi::top());
  "-1 2 +"_fs;
  TEST_ASSERT_EQUAL(1, shi::top());
  "-1 -2 +"_fs;
  TEST_ASSERT_EQUAL(-3, shi::top());
  "-1 1 +"_fs;
  TEST_ASSERT_EQUAL(0, shi::top());
  "MID-UINT 1 +"_fs;
  TEST_ASSERT_EQUAL(MID_UINT_p1, shi::top());

  "0 5 -"_fs;
  TEST_ASSERT_EQUAL(-5, shi::top());
  "5 0 -"_fs;
  TEST_ASSERT_EQUAL(5, shi::top());
  "0 -5 -"_fs;
  TEST_ASSERT_EQUAL(5, shi::top());
  "-5 0 -"_fs;
  TEST_ASSERT_EQUAL(-5, shi::top());
  "1 2 -"_fs;
  TEST_ASSERT_EQUAL(-1, shi::top());
  "1 -2 -"_fs;
  TEST_ASSERT_EQUAL(3, shi::top());
  "-1 2 -"_fs;
  TEST_ASSERT_EQUAL(-3, shi::top());
  "-1 -2 -"_fs;
  TEST_ASSERT_EQUAL(1, shi::top());
  "0 1 -"_fs;
  TEST_ASSERT_EQUAL(-1, shi::top());
  "MID-UINT+1 1 -"_fs;
  TEST_ASSERT_EQUAL(MID_UINT, shi::top());

  "0 1+"_fs;
  TEST_ASSERT_EQUAL(1, shi::top());
  "-1 1+"_fs;
  TEST_ASSERT_EQUAL(0, shi::top());
  "1 1+"_fs;
  TEST_ASSERT_EQUAL(2, shi::top());
  "MID-UINT 1+"_fs;
  TEST_ASSERT_EQUAL(MID_UINT_p1, shi::top());

  "2 1-"_fs;
  TEST_ASSERT_EQUAL(1, shi::top());
  "1 1-"_fs;
  TEST_ASSERT_EQUAL(0, shi::top());
  "0 1-"_fs;
  TEST_ASSERT_EQUAL(-1, shi::top());
  "MID-UINT+1 1-"_fs;
  TEST_ASSERT_EQUAL(MID_UINT, shi::top());

  "0 negate"_fs;
  TEST_ASSERT_EQUAL(0, shi::top());
  "1 negate"_fs;
  TEST_ASSERT_EQUAL(-1, shi::top());
  "-1 negate"_fs;
  TEST_ASSERT_EQUAL(1, shi::top());
  "2 negate"_fs;
  TEST_ASSERT_EQUAL(-2, shi::top());
  "-2 negate"_fs;
  TEST_ASSERT_EQUAL(2, shi::top());

  "0 abs"_fs;
  TEST_ASSERT_EQUAL(0, shi::top());
  "1 abs"_fs;
  TEST_ASSERT_EQUAL(1, shi::top());
  "-1 abs"_fs;
  TEST_ASSERT_EQUAL(1, shi::top());
  "MIN-INT abs"_fs;
  TEST_ASSERT_EQUAL(MID_UINT_p1, shi::top());

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
  //  HERE 1 ALLOT
  //  HERE
  //  CONSTANT 2NDA
  //  CONSTANT 1STA
  //  T{ 1STA 2NDA U< -> <TRUE> }T      \ HERE MUST GROW WITH ALLOT
  //  T{ 1STA 1+ -> 2NDA }T         \ ... BY ONE ADDRESS UNIT
  //  ( MISSING TEST: NEGATIVE ALLOT )
  //
  //  \ Added by GWJ so that ALIGN can be used before , (comma) is tested
  //  1 ALIGNED CONSTANT ALMNT   \ -- 1|2|4|8 for 8|16|32|64 bit alignment
  //  ALIGN
  //  T{ HERE 1 ALLOT ALIGN HERE SWAP - ALMNT = -> <TRUE> }T
  //  \ End of extra test
  //
  //  HERE 1 ,
  //  HERE 2 ,
  //  CONSTANT 2ND
  //  CONSTANT 1ST
  //  T{ 1ST 2ND U< -> <TRUE> }T         \ HERE MUST GROW WITH ALLOT
  //  T{ 1ST CELL+ -> 2ND }T         \ ... BY ONE CELL
  //  T{ 1ST 1 CELLS + -> 2ND }T
  //  T{ 1ST @ 2ND @ -> 1 2 }T
  //  T{ 5 1ST ! -> }T
  //  T{ 1ST @ 2ND @ -> 5 2 }T
  //  T{ 6 2ND ! -> }T
  //  T{ 1ST @ 2ND @ -> 5 6 }T
  //  T{ 1ST 2@ -> 6 5 }T
  //  T{ 2 1 1ST 2! -> }T
  //  T{ 1ST 2@ -> 2 1 }T
  //  T{ 1S 1ST !  1ST @ -> 1S }T      \ CAN STORE CELL-WIDE VALUE
  //
  //  HERE 1 C,
  //  HERE 2 C,
  //  CONSTANT 2NDC
  //  CONSTANT 1STC
  //  T{ 1STC 2NDC U< -> <TRUE> }T      \ HERE MUST GROW WITH ALLOT
  //  T{ 1STC CHAR+ -> 2NDC }T         \ ... BY ONE CHAR
  //  T{ 1STC 1 CHARS + -> 2NDC }T
  //  T{ 1STC C@ 2NDC C@ -> 1 2 }T
  //  T{ 3 1STC C! -> }T
  //  T{ 1STC C@ 2NDC C@ -> 3 2 }T
  //  T{ 4 2NDC C! -> }T
  //  T{ 1STC C@ 2NDC C@ -> 3 4 }T
  //
  //  ALIGN 1 ALLOT HERE ALIGN HERE 3 CELLS ALLOT
  //  CONSTANT A-ADDR  CONSTANT UA-ADDR
  //  T{ UA-ADDR ALIGNED -> A-ADDR }T
  //  T{    1 A-ADDR C!  A-ADDR C@ ->    1 }T
  //  T{ 1234 A-ADDR  !  A-ADDR  @ -> 1234 }T
  //  T{ 123 456 A-ADDR 2!  A-ADDR 2@ -> 123 456 }T
  //  T{ 2 A-ADDR CHAR+ C!  A-ADDR CHAR+ C@ -> 2 }T
  //  T{ 3 A-ADDR CELL+ C!  A-ADDR CELL+ C@ -> 3 }T
  //  T{ 1234 A-ADDR CELL+ !  A-ADDR CELL+ @ -> 1234 }T
  //  T{ 123 456 A-ADDR CELL+ 2!  A-ADDR CELL+ 2@ -> 123 456 }T
  //
  //  : BITS ( X -- U )
  //     0 SWAP BEGIN DUP WHILE DUP MSB AND IF >R 1+ R> THEN 2* REPEAT DROP ;
  //  ( CHARACTERS >= 1 AU, <= SIZE OF CELL, >= 8 BITS )
  //  T{ 1 CHARS 1 < -> <FALSE> }T
  //  T{ 1 CHARS 1 CELLS > -> <FALSE> }T
  //  ( TBD: HOW TO FIND NUMBER OF BITS? )
  //
  //  ( CELLS >= 1 AU, INTEGRAL MULTIPLE OF CHAR SIZE, >= 16 BITS )
  //  T{ 1 CELLS 1 < -> <FALSE> }T
  //  T{ 1 CELLS 1 CHARS MOD -> 0 }T
  //  T{ 1S BITS 10 < -> <FALSE> }T
  //
  //  T{ 0 1ST ! -> }T
  //  T{ 1 1ST +! -> }T
  //  T{ 1ST @ -> 1 }T
  //  T{ -1 1ST +! 1ST @ -> 0 }T
}

TEST(
    tick__bracket_tick__find__execute__immediate__count__literal__postpone__state) {
  //  T{ : GT1 123 ; -> }T
  //  T{ ' GT1 EXECUTE -> 123 }T
  //  T{ : GT2 ['] GT1 ; IMMEDIATE -> }T
  //  T{ GT2 EXECUTE -> 123 }T
  //  HERE 3 C, CHAR G C, CHAR T C, CHAR 1 C, CONSTANT GT1STRING
  //  HERE 3 C, CHAR G C, CHAR T C, CHAR 2 C, CONSTANT GT2STRING
  //  T{ GT1STRING FIND -> ' GT1 -1 }T
  //  T{ GT2STRING FIND -> ' GT2 1 }T
  //  ( HOW TO SEARCH FOR NON-EXISTENT WORD? )
  //  T{ : GT3 GT2 LITERAL ; -> }T
  //  T{ GT3 -> ' GT1 }T
  //  T{ GT1STRING COUNT -> GT1STRING CHAR+ 3 }T
  //
  //  T{ : GT4 POSTPONE GT1 ; IMMEDIATE -> }T
  //  T{ : GT5 GT4 ; -> }T
  //  T{ GT5 -> 123 }T
  //  T{ : GT6 345 ; IMMEDIATE -> }T
  //  T{ : GT7 POSTPONE GT6 ; -> }T
  //  T{ GT7 -> 345 }T
  //
  //  T{ : GT8 STATE @ ; IMMEDIATE -> }T
  //  T{ GT8 -> 0 }T
  //  T{ : GT9 GT8 LITERAL ; -> }T
  //  T{ GT9 0= -> <FALSE> }T
}

TEST(if__else__then__begin__while__repeat__until__recurse) {
  //  T{ : GI1 IF 123 THEN ; -> }T
  //  T{ : GI2 IF 123 ELSE 234 THEN ; -> }T
  //  T{ 0 GI1 -> }T
  //  T{ 1 GI1 -> 123 }T
  //  T{ -1 GI1 -> 123 }T
  //  T{ 0 GI2 -> 234 }T
  //  T{ 1 GI2 -> 123 }T
  //  T{ -1 GI1 -> 123 }T
  //
  //  T{ : GI3 BEGIN DUP 5 < WHILE DUP 1+ REPEAT ; -> }T
  //  T{ 0 GI3 -> 0 1 2 3 4 5 }T
  //  T{ 4 GI3 -> 4 5 }T
  //  T{ 5 GI3 -> 5 }T
  //  T{ 6 GI3 -> 6 }T
  //
  //  T{ : GI4 BEGIN DUP 1+ DUP 5 > UNTIL ; -> }T
  //  T{ 3 GI4 -> 3 4 5 6 }T
  //  T{ 5 GI4 -> 5 6 }T
  //  T{ 6 GI4 -> 6 7 }T
  //
  //  T{ : GI5 BEGIN DUP 2 >
  //           WHILE DUP 5 < WHILE DUP 1+ REPEAT 123 ELSE 345 THEN ; -> }T
  //  T{ 1 GI5 -> 1 345 }T
  //  T{ 2 GI5 -> 2 345 }T
  //  T{ 3 GI5 -> 3 4 5 123 }T
  //  T{ 4 GI5 -> 4 5 123 }T
  //  T{ 5 GI5 -> 5 123 }T
  //
  //  T{ : GI6 ( N -- 0,1,..N ) DUP IF DUP >R 1- RECURSE R> THEN ; -> }T
  //  T{ 0 GI6 -> 0 }T
  //  T{ 1 GI6 -> 0 1 }T
  //  T{ 2 GI6 -> 0 1 2 }T
  //  T{ 3 GI6 -> 0 1 2 3 }T
  //  T{ 4 GI6 -> 0 1 2 3 4 }T
}

TEST(do__loop__plus_loop__i__j__unloop__leave__exit) {
  //  T{ : GD1 DO I LOOP ; -> }T
  //  T{ 4 1 GD1 -> 1 2 3 }T
  //  T{ 2 -1 GD1 -> -1 0 1 }T
  //  T{ MID-UINT+1 MID-UINT GD1 -> MID-UINT }T
  //
  //  T{ : GD2 DO I -1 +LOOP ; -> }T
  //  T{ 1 4 GD2 -> 4 3 2 1 }T
  //  T{ -1 2 GD2 -> 2 1 0 -1 }T
  //  T{ MID-UINT MID-UINT+1 GD2 -> MID-UINT+1 MID-UINT }T
  //
  //  T{ : GD3 DO 1 0 DO J LOOP LOOP ; -> }T
  //  T{ 4 1 GD3 -> 1 2 3 }T
  //  T{ 2 -1 GD3 -> -1 0 1 }T
  //  T{ MID-UINT+1 MID-UINT GD3 -> MID-UINT }T
  //
  //  T{ : GD4 DO 1 0 DO J LOOP -1 +LOOP ; -> }T
  //  T{ 1 4 GD4 -> 4 3 2 1 }T
  //  T{ -1 2 GD4 -> 2 1 0 -1 }T
  //  T{ MID-UINT MID-UINT+1 GD4 -> MID-UINT+1 MID-UINT }T
  //
  //  T{ : GD5 123 SWAP 0 DO I 4 > IF DROP 234 LEAVE THEN LOOP ; -> }T
  //  T{ 1 GD5 -> 123 }T
  //  T{ 5 GD5 -> 123 }T
  //  T{ 6 GD5 -> 234 }T
  //
  //  T{ : GD6  ( PAT: T{0 0},{0 0}{1 0}{1 1},{0 0}{1 0}{1 1}{2 0}{2 1}{2 2} )
  //     0 SWAP 0 DO
  //        I 1+ 0 DO I J + 3 = IF I UNLOOP I UNLOOP EXIT THEN 1+ LOOP
  //      LOOP ; -> }T
  //  T{ 1 GD6 -> 1 }T
  //  T{ 2 GD6 -> 3 }T
  //  T{ 3 GD6 -> 4 1 2 }T
}

TEST(colon__semicolon__constant__variable__create__does__to_body) {
  //  T{ 123 CONSTANT X123 -> }T
  //  T{ X123 -> 123 }T
  //  T{ : EQU CONSTANT ; -> }T
  //  T{ X123 EQU Y123 -> }T
  //  T{ Y123 -> 123 }T
  //
  //  T{ VARIABLE V1 -> }T
  //  T{ 123 V1 ! -> }T
  //  T{ V1 @ -> 123 }T
  //
  //  T{ : NOP : POSTPONE ; ; -> }T
  //  T{ NOP NOP1 NOP NOP2 -> }T
  //  T{ NOP1 -> }T
  //  T{ NOP2 -> }T
  //
  //  T{ : DOES1 DOES> @ 1 + ; -> }T
  //  T{ : DOES2 DOES> @ 2 + ; -> }T
  //  T{ CREATE CR1 -> }T
  //  T{ CR1 -> HERE }T
  //  T{ ' CR1 >BODY -> HERE }T
  //  T{ 1 , -> }T
  //  T{ CR1 @ -> 1 }T
  //  T{ DOES1 -> }T
  //  T{ CR1 -> 2 }T
  //  T{ DOES2 -> }T
  //  T{ CR1 -> 3 }T
  //
  //  T{ : WEIRD: CREATE DOES> 1 + DOES> 2 + ; -> }T
  //  T{ WEIRD: W1 -> }T
  //  T{ ' W1 >BODY -> HERE }T
  //  T{ W1 -> HERE 1 + }T
  //  T{ W1 -> HERE 2 + }T
}

TEST(source__to_in__word) {
  //  : GS1 S" SOURCE" 2DUP EVALUATE
  //         >R SWAP >R = R> R> = ;
  //  T{ GS1 -> <TRUE> <TRUE> }T
  //
  //  VARIABLE SCANS
  //  : RESCAN?  -1 SCANS +! SCANS @ IF 0 >IN ! THEN ;
  //
  //  T{ 2 SCANS !
  //  345 RESCAN?
  //  -> 345 345 }T
  //
  //  : GS2  5 SCANS ! S" 123 RESCAN?" EVALUATE ;
  //  T{ GS2 -> 123 123 123 123 123 }T
  //
  //  : GS3 WORD COUNT SWAP C@ ;
  //  T{ BL GS3 HELLO -> 5 CHAR H }T
  //  T{ CHAR " GS3 GOODBYE" -> 7 CHAR G }T
  //  T{ BL GS3
  //  DROP -> 0 }T            \ BLANK LINE RETURN ZERO-LENGTH STRING
  //
  //  : GS4 SOURCE >IN ! DROP ;
  //  T{ GS4 123 456
  //  -> }T
}

TEST(num_start__num__num_s__hold__sign__base__to_number__hex__decimal) {
  //  : S=  \ ( ADDR1 C1 ADDR2 C2 -- T/F ) COMPARE TWO STRINGS.
  //     >R SWAP R@ = IF         \ MAKE SURE STRINGS HAVE SAME LENGTH
  //        R> ?DUP IF         \ IF NON-EMPTY STRINGS
  //      0 DO
  //         OVER C@ OVER C@ - IF 2DROP <FALSE> UNLOOP EXIT THEN
  //         SWAP CHAR+ SWAP CHAR+
  //           LOOP
  //        THEN
  //        2DROP <TRUE>         \ IF WE GET HERE, STRINGS MATCH
  //     ELSE
  //        R> DROP 2DROP <FALSE>      \ LENGTHS MISMATCH
  //     THEN ;
  //
  //  : GP1  <# 41 HOLD 42 HOLD 0 0 #> S" BA" S= ;
  //  T{ GP1 -> <TRUE> }T
  //
  //  : GP2  <# -1 SIGN 0 SIGN -1 SIGN 0 0 #> S" --" S= ;
  //  T{ GP2 -> <TRUE> }T
  //
  //  : GP3  <# 1 0 # # #> S" 01" S= ;
  //  T{ GP3 -> <TRUE> }T
  //
  //  : GP4  <# 1 0 #S #> S" 1" S= ;
  //  T{ GP4 -> <TRUE> }T
  //
  //  24 CONSTANT MAX-BASE         \ BASE 2 .. 36
  //  : COUNT-BITS
  //     0 0 INVERT BEGIN DUP WHILE >R 1+ R> 2* REPEAT DROP ;
  //  COUNT-BITS 2* CONSTANT #BITS-UD      \ NUMBER OF BITS IN UD
  //
  //  : GP5
  //     BASE @ <TRUE>
  //     MAX-BASE 1+ 2 DO         \ FOR EACH POSSIBLE BASE
  //        I BASE !            \ TBD: ASSUMES BASE WORKS
  //        I 0 <# #S #> S" 10" S= AND
  //     LOOP
  //     SWAP BASE ! ;
  //  T{ GP5 -> <TRUE> }T
  //
  //  : GP6
  //     BASE @ >R  2 BASE !
  //     MAX-UINT MAX-UINT <# #S #>      \ MAXIMUM UD TO BINARY
  //     R> BASE !            \ S: C-ADDR U
  //     DUP #BITS-UD = SWAP
  //     0 DO               \ S: C-ADDR FLAG
  //        OVER C@ [CHAR] 1 = AND      \ ALL ONES
  //        >R CHAR+ R>
  //     LOOP SWAP DROP ;
  //  T{ GP6 -> <TRUE> }T
  //
  //  : GP7
  //     BASE @ >R    MAX-BASE BASE !
  //     <TRUE>
  //     A 0 DO
  //        I 0 <# #S #>
  //        1 = SWAP C@ I 30 + = AND AND
  //     LOOP
  //     MAX-BASE A DO
  //        I 0 <# #S #>
  //        1 = SWAP C@ 41 I A - + = AND AND
  //     LOOP
  //     R> BASE ! ;
  //
  //  T{ GP7 -> <TRUE> }T
  //
  //  \ >NUMBER TESTS
  //  CREATE GN-BUF 0 C,
  //  : GN-STRING   GN-BUF 1 ;
  //  : GN-CONSUMED   GN-BUF CHAR+ 0 ;
  //  : GN'      [CHAR] ' WORD CHAR+ C@ GN-BUF C!  GN-STRING ;
  //
  //  T{ 0 0 GN' 0' >NUMBER -> 0 0 GN-CONSUMED }T
  //  T{ 0 0 GN' 1' >NUMBER -> 1 0 GN-CONSUMED }T
  //  T{ 1 0 GN' 1' >NUMBER -> BASE @ 1+ 0 GN-CONSUMED }T
  //  T{ 0 0 GN' -' >NUMBER -> 0 0 GN-STRING }T   \ SHOULD FAIL TO CONVERT THESE
  //  T{ 0 0 GN' +' >NUMBER -> 0 0 GN-STRING }T
  //  T{ 0 0 GN' .' >NUMBER -> 0 0 GN-STRING }T
  //
  //  : >NUMBER-BASED
  //     BASE @ >R BASE ! >NUMBER R> BASE ! ;
  //
  //  T{ 0 0 GN' 2' 10 >NUMBER-BASED -> 2 0 GN-CONSUMED }T
  //  T{ 0 0 GN' 2'  2 >NUMBER-BASED -> 0 0 GN-STRING }T
  //  T{ 0 0 GN' F' 10 >NUMBER-BASED -> F 0 GN-CONSUMED }T
  //  T{ 0 0 GN' G' 10 >NUMBER-BASED -> 0 0 GN-STRING }T
  //  T{ 0 0 GN' G' MAX-BASE >NUMBER-BASED -> 10 0 GN-CONSUMED }T
  //  T{ 0 0 GN' Z' MAX-BASE >NUMBER-BASED -> 23 0 GN-CONSUMED }T
  //
  //  : GN1   \ ( UD BASE -- UD' LEN ) UD SHOULD EQUAL UD' AND LEN SHOULD BE
  //  ZERO.
  //     BASE @ >R BASE !
  //     <# #S #>
  //     0 0 2SWAP >NUMBER SWAP DROP      \ RETURN LENGTH ONLY
  //     R> BASE ! ;
  //  T{ 0 0 2 GN1 -> 0 0 0 }T
  //  T{ MAX-UINT 0 2 GN1 -> MAX-UINT 0 0 }T
  //  T{ MAX-UINT DUP 2 GN1 -> MAX-UINT DUP 0 }T
  //  T{ 0 0 MAX-BASE GN1 -> 0 0 0 }T
  //  T{ MAX-UINT 0 MAX-BASE GN1 -> MAX-UINT 0 0 }T
  //  T{ MAX-UINT DUP MAX-BASE GN1 -> MAX-UINT DUP 0 }T
  //
  //  : GN2   \ ( -- 16 10 )
  //     BASE @ >R  HEX BASE @  DECIMAL BASE @  R> BASE ! ;
  //  T{ GN2 -> 10 A }T
}
