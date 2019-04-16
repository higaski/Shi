#include "forth2012_test_suite.hpp"

using namespace shi::literals;

TEST(basic_assumptions) {
  // ( TEST IF ANY BITS ARE SET; ANSWER IN BASE 1 )
  ": BITSSET? if 0 0 else 0 then ;"_s;
  "0 BITSSET?"_s;  // ( ZERO IS ALL BITS CLEAR )
  TEST_ASSERT_EQUAL(1, shi::size());
  TEST_ASSERT_EQUAL(0, shi::top());
  "1 BITSSET?"_s;  // ( OTHER NUMBER HAVE AT LEAST ONE BIT )
  TEST_ASSERT_EQUAL(3, shi::size());
  TEST_ASSERT_EQUAL(0, shi::top());
  TEST_ASSERT_EQUAL(0, shi::top(1));
  TEST_ASSERT_EQUAL(0, shi::top(2));
  "-1 BITSSET?"_s;
  TEST_ASSERT_EQUAL(5, shi::size());
  TEST_ASSERT_EQUAL(0, shi::top());
  TEST_ASSERT_EQUAL(0, shi::top(1));
  TEST_ASSERT_EQUAL(0, shi::top(2));
  TEST_ASSERT_EQUAL(0, shi::top(3));
  TEST_ASSERT_EQUAL(0, shi::top(4));

  shi::clear();
}

TEST(invert__and__or__xor) {
  "0 0 and"_s;
  TEST_ASSERT_EQUAL(0, shi::top());
  "0 1 and"_s;
  TEST_ASSERT_EQUAL(0, shi::top());
  "1 0 and"_s;
  TEST_ASSERT_EQUAL(0, shi::top());
  "1 1 and"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  "0 invert 1 and"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  "1 invert 1 and"_s;
  TEST_ASSERT_EQUAL(0, shi::top());

  "0 constant 0S"_s;
  "0 invert constant 1S"_s;

  "0S invert"_s;
  TEST_ASSERT_EQUAL(_1S, shi::top());
  "1S invert"_s;
  TEST_ASSERT_EQUAL(_0S, shi::top());

  "0S 0S or"_s;
  TEST_ASSERT_EQUAL(_0S, shi::top());
  "0S 1S or"_s;
  TEST_ASSERT_EQUAL(_1S, shi::top());
  "1S 0S or"_s;
  TEST_ASSERT_EQUAL(_1S, shi::top());
  "1S 1S or"_s;
  TEST_ASSERT_EQUAL(_1S, shi::top());

  "0S 0S xor"_s;
  TEST_ASSERT_EQUAL(_0S, shi::top());
  "0S 1S xor"_s;
  TEST_ASSERT_EQUAL(_1S, shi::top());
  "1S 0S xor"_s;
  TEST_ASSERT_EQUAL(_1S, shi::top());
  "1S 1S xor"_s;
  TEST_ASSERT_EQUAL(_0S, shi::top());

  shi::clear();
}

TEST(two_times__two_div__lshift__rshit) {
  // ( WE TRUST 1S, INVERT, AND BITSSET?; WE WILL CONFIRM RSHIFT LATER )
  "1S 1 rshift invert constant MSB"_s;
  "MSB BITSSET?"_s;
  TEST_ASSERT_EQUAL(2, shi::size());
  TEST_ASSERT_EQUAL(0, shi::top());
  TEST_ASSERT_EQUAL(0, shi::top(1));

  "0S 2*"_s;
  TEST_ASSERT_EQUAL(_0S, shi::top());
  "1 2*"_s;
  TEST_ASSERT_EQUAL(2, shi::top());
  "4000 2*"_s;
  TEST_ASSERT_EQUAL(8000, shi::top());
  "1S 2* 1 xor"_s;
  TEST_ASSERT_EQUAL(_1S, shi::top());
  "MSB 2*"_s;
  TEST_ASSERT_EQUAL(_0S, shi::top());

  "0S 2/"_s;
  TEST_ASSERT_EQUAL(_0S, shi::top());
  "1 2/"_s;
  TEST_ASSERT_EQUAL(0, shi::top());
  "4000 2/"_s;
  TEST_ASSERT_EQUAL(2000, shi::top());
  "1S 2/"_s;  // \ MSB PROPOGATED
  TEST_ASSERT_EQUAL(_1S, shi::top());
  "1S 1 xor 2/"_s;
  TEST_ASSERT_EQUAL(_1S, shi::top());
  "MSB 2/ MSB and"_s;
  TEST_ASSERT_EQUAL(MSB, shi::top());

  "1 0 lshift"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  "1 1 lshift"_s;
  TEST_ASSERT_EQUAL(2, shi::top());
  "1 2 lshift"_s;
  TEST_ASSERT_EQUAL(4, shi::top());
  // T{ 1 F LSHIFT -> 8000 }T \ BIGGEST GUARANTEED SHIFT
  "1S 1 lshift 1 xor"_s;
  TEST_ASSERT_EQUAL(_1S, shi::top());
  "MSB 1 lshift"_s;
  TEST_ASSERT_EQUAL(0, shi::top());

  "1 0 rshift"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  "1 1 rshift"_s;
  TEST_ASSERT_EQUAL(0, shi::top());
  "2 1 rshift"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  "4 2 rshift"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  // T{ 8000 F RSHIFT -> 1 }T \ BIGGEST
  "MSB 1 rshift MSB and"_s;  // \ RSHIFT ZERO FILLS MSBS
  TEST_ASSERT_EQUAL(0, shi::top());
  "MSB 1 rshift 2*"_s;
  TEST_ASSERT_EQUAL(MSB, shi::top());

  shi::clear();
}

TEST(zero_equal__equal__zero_less__less__more__u_less__min__max) {
  "0 invert constant MAX-UINT"_s;
  "0 invert 1 rshift constant MAX-INT"_s;
  "0 invert 1 rshift invert constant MIN-INT"_s;
  "0 invert 1 rshift constant MID-UINT"_s;
  "0 invert 1 rshift invert constant MID-UINT+1"_s;
  "0S constant <FALSE>"_s;
  "1S constant <TRUE>"_s;

  "0 0="_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "1 0="_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "2 0="_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "-1 0="_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "MAX-UINT 0="_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "MIN-INT 0="_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "MAX-INT 0="_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());

  "0 0 ="_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "1 1 ="_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "-1 -1 ="_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "1 0 ="_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "-1 0 ="_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "0 1 ="_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "0 -1 ="_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());

  "0 0<"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "-1 0<"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "MIN-INT 0<"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "1 0<"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "MAX-INT 0<"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());

  "0 1 <"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "1 2 <"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "-1 0 <"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "-1 1 <"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "MIN-INT 0 <"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "MIN-INT MAX-INT <"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "0 MAX-INT <"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "0 0 <"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "1 1 <"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "1 0 <"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "2 1 <"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "0 -1 <"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "1 -1 <"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "0 MIN-INT <"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "MAX-INT MIN-INT <"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "MAX-INT 0 <"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());

  "0 1 >"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "1 2 >"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "-1 0 >"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "-1 1 >"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "MIN-INT 0 >"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "MIN-INT MAX-INT >"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "0 MAX-INT >"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "0 0 >"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "1 1 >"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "1 0 >"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "2 1 >"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "0 -1 >"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "1 -1 >"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "0 MIN-INT >"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "MAX-INT MIN-INT >"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "MAX-INT 0 >"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());

  shi::clear();

  "0 1 u<"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "1 2 u<"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "0 MID-UINT u<"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "0 MAX-UINT u<"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "MID-UINT MAX-UINT u<"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "0 0 u<"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "1 1 u<"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "1 0 u<"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "2 1 u<"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "MID-UINT 0 u<"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "MAX-UINT 0 u<"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "MAX-UINT MID-UINT u<"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());

  "0 1 min"_s;
  TEST_ASSERT_EQUAL(0, shi::top());
  "1 2 min"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  "-1 0 min"_s;
  TEST_ASSERT_EQUAL(-1, shi::top());
  "-1 1 min"_s;
  TEST_ASSERT_EQUAL(-1, shi::top());
  "MIN-INT 0 min"_s;
  TEST_ASSERT_EQUAL(MIN_INT, shi::top());
  "MIN-INT MAX-INT min"_s;
  TEST_ASSERT_EQUAL(MIN_INT, shi::top());
  "0 MAX-INT min"_s;
  TEST_ASSERT_EQUAL(0, shi::top());
  "0 0 min"_s;
  TEST_ASSERT_EQUAL(0, shi::top());
  "1 1 min"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  "1 0 min"_s;
  TEST_ASSERT_EQUAL(0, shi::top());
  "2 1 min"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  "0 -1 min"_s;
  TEST_ASSERT_EQUAL(-1, shi::top());
  "1 -1 min"_s;
  TEST_ASSERT_EQUAL(-1, shi::top());
  "0 MIN-INT min"_s;
  TEST_ASSERT_EQUAL(MIN_INT, shi::top());
  "MAX-INT MIN-INT min"_s;
  TEST_ASSERT_EQUAL(MIN_INT, shi::top());
  "MAX-INT 0 min"_s;
  TEST_ASSERT_EQUAL(0, shi::top());

  "0 1 max"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  "1 2 max"_s;
  "-1 0 max"_s;
  TEST_ASSERT_EQUAL(0, shi::top());
  "-1 1 max"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  "MIN-INT 0 max"_s;
  TEST_ASSERT_EQUAL(0, shi::top());
  "MIN-INT MAX-INT max"_s;
  TEST_ASSERT_EQUAL(MAX_INT, shi::top());
  "0 MAX-INT max"_s;
  TEST_ASSERT_EQUAL(MAX_INT, shi::top());
  "0 0 max"_s;
  TEST_ASSERT_EQUAL(0, shi::top());
  "1 1 max"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  "1 0 max"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  "2 1 max"_s;
  "0 -1 max"_s;
  TEST_ASSERT_EQUAL(0, shi::top());
  "1 -1 max"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  "0 MIN-INT max"_s;
  TEST_ASSERT_EQUAL(0, shi::top());
  "MAX-INT MIN-INT max"_s;
  TEST_ASSERT_EQUAL(MAX_INT, shi::top());
  "MAX-INT 0 max"_s;
  TEST_ASSERT_EQUAL(MAX_INT, shi::top());

  shi::clear();
}

TEST(__2drop__2dup__2over__2swap_q_dup__depth__drop__dup__over__rot__swap) {
  "1 2 2drop"_s;
  TEST_ASSERT_EQUAL(0, shi::size());

  "1 2 2dup"_s;
  TEST_ASSERT_EQUAL(4, shi::size());
  TEST_ASSERT_EQUAL(2, shi::top());
  TEST_ASSERT_EQUAL(1, shi::top(1));
  TEST_ASSERT_EQUAL(2, shi::top(2));
  TEST_ASSERT_EQUAL(1, shi::top(3));

  "1 2 3 4 2over"_s;
  TEST_ASSERT_EQUAL(10, shi::size());
  TEST_ASSERT_EQUAL(2, shi::top());
  TEST_ASSERT_EQUAL(1, shi::top(1));
  TEST_ASSERT_EQUAL(4, shi::top(2));
  TEST_ASSERT_EQUAL(3, shi::top(3));
  TEST_ASSERT_EQUAL(2, shi::top(4));
  TEST_ASSERT_EQUAL(1, shi::top(5));
  TEST_ASSERT_EQUAL(2, shi::top(6));
  TEST_ASSERT_EQUAL(1, shi::top(7));
  TEST_ASSERT_EQUAL(2, shi::top(8));
  TEST_ASSERT_EQUAL(1, shi::top(9));

  shi::clear();

  "1 2 3 4 2swap"_s;
  TEST_ASSERT_EQUAL(4, shi::size());
  TEST_ASSERT_EQUAL(2, shi::top());
  TEST_ASSERT_EQUAL(1, shi::top(1));
  TEST_ASSERT_EQUAL(4, shi::top(2));
  TEST_ASSERT_EQUAL(3, shi::top(3));

  "0 ?dup"_s;
  TEST_ASSERT_EQUAL(0, shi::top());

  "1 ?dup"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  TEST_ASSERT_EQUAL(1, shi::top(1));

  "-1 ?dup"_s;
  TEST_ASSERT_EQUAL(-1, shi::top());
  TEST_ASSERT_EQUAL(-1, shi::top(1));

  shi::clear();

  "depth"_s;
  TEST_ASSERT_EQUAL(0, shi::top());

  shi::clear();

  "0 depth"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  TEST_ASSERT_EQUAL(0, shi::top(1));

  shi::clear();

  "0 1 depth"_s;
  TEST_ASSERT_EQUAL(2, shi::top());
  TEST_ASSERT_EQUAL(1, shi::top(1));
  TEST_ASSERT_EQUAL(0, shi::top(2));

  shi::clear();

  "0 drop"_s;
  TEST_ASSERT_EQUAL(0, shi::size());

  "1 2 drop"_s;
  TEST_ASSERT_EQUAL(1, shi::size());
  TEST_ASSERT_EQUAL(1, shi::top());

  "1 dup"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  TEST_ASSERT_EQUAL(1, shi::top(1));

  "1 2 over"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  TEST_ASSERT_EQUAL(2, shi::top(1));
  TEST_ASSERT_EQUAL(1, shi::top(2));

  "1 2 3 rot"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  TEST_ASSERT_EQUAL(3, shi::top(1));
  TEST_ASSERT_EQUAL(2, shi::top(2));

  "1 2 swap"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  TEST_ASSERT_EQUAL(2, shi::top(1));

  shi::clear();
}

TEST(to_r__from_r__r_fetch) {
  ": GR1 >r r> ;"_s;
  ": GR2 >r r@ r> drop ;"_s;

  "123 GR1"_s;
  TEST_ASSERT_EQUAL(123, shi::top());

  "123 GR2"_s;
  TEST_ASSERT_EQUAL(123, shi::top());
  TEST_ASSERT_EQUAL(123, shi::top(1));

  "1S GR1"_s;  // ( RETURN STACK HOLDS CELLS )
  TEST_ASSERT_EQUAL(_1S, shi::top());

  shi::clear();
}

TEST(plus__minus__one_plus__one_minus__abs__negate) {
  "0 5 +"_s;
  TEST_ASSERT_EQUAL(5, shi::top());
  "5 0 +"_s;
  TEST_ASSERT_EQUAL(5, shi::top());
  "0 -5 +"_s;
  TEST_ASSERT_EQUAL(-5, shi::top());
  "-5 0 +"_s;
  TEST_ASSERT_EQUAL(-5, shi::top());
  "1 2 +"_s;
  TEST_ASSERT_EQUAL(3, shi::top());
  "1 -2 +"_s;
  TEST_ASSERT_EQUAL(-1, shi::top());
  "-1 2 +"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  "-1 -2 +"_s;
  TEST_ASSERT_EQUAL(-3, shi::top());
  "-1 1 +"_s;
  TEST_ASSERT_EQUAL(0, shi::top());
  "MID-UINT 1 +"_s;
  TEST_ASSERT_EQUAL_UINT32(MID_UINT_p1, shi::top());

  "0 5 -"_s;
  TEST_ASSERT_EQUAL(-5, shi::top());
  "5 0 -"_s;
  TEST_ASSERT_EQUAL(5, shi::top());
  "0 -5 -"_s;
  TEST_ASSERT_EQUAL(5, shi::top());
  "-5 0 -"_s;
  TEST_ASSERT_EQUAL(-5, shi::top());
  "1 2 -"_s;
  TEST_ASSERT_EQUAL(-1, shi::top());
  "1 -2 -"_s;
  TEST_ASSERT_EQUAL(3, shi::top());
  "-1 2 -"_s;
  TEST_ASSERT_EQUAL(-3, shi::top());
  "-1 -2 -"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  "0 1 -"_s;
  TEST_ASSERT_EQUAL(-1, shi::top());
  "MID-UINT+1 1 -"_s;
  TEST_ASSERT_EQUAL(MID_UINT, shi::top());

  "0 1+"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  "-1 1+"_s;
  TEST_ASSERT_EQUAL(0, shi::top());
  "1 1+"_s;
  TEST_ASSERT_EQUAL(2, shi::top());
  "MID-UINT 1+"_s;
  TEST_ASSERT_EQUAL_UINT32(MID_UINT_p1, shi::top());

  "2 1-"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  "1 1-"_s;
  TEST_ASSERT_EQUAL(0, shi::top());
  "0 1-"_s;
  TEST_ASSERT_EQUAL(-1, shi::top());
  "MID-UINT+1 1-"_s;
  TEST_ASSERT_EQUAL(MID_UINT, shi::top());

  "0 negate"_s;
  TEST_ASSERT_EQUAL(0, shi::top());
  "1 negate"_s;
  TEST_ASSERT_EQUAL(-1, shi::top());
  "-1 negate"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  "2 negate"_s;
  TEST_ASSERT_EQUAL(-2, shi::top());
  "-2 negate"_s;
  TEST_ASSERT_EQUAL(2, shi::top());

  "0 abs"_s;
  TEST_ASSERT_EQUAL(0, shi::top());
  "1 abs"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  "-1 abs"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  "MIN-INT abs"_s;
  TEST_ASSERT_EQUAL_UINT32(MID_UINT_p1, shi::top());

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
    here__comma__fetch__store__cell_plus__cells__c_comma__c_fetch__c_store__chars__two_fetch__two_store__align__aligned__plus_store__allot) {
  "here 1 allot"_s;
  "here"_s;
  "constant 2NDA"_s;
  "constant 1STA"_s;
  "1STA 2NDA u<"_s;
  TEST_ASSERT_EQUAL(1, shi::size());
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "1STA 1+"_s;
  "2NDA"_s;
  TEST_ASSERT_EQUAL(shi::top(), shi::top(1));
  // ( MISSING TEST: NEGATIVE ALLOT )

  "1 aligned constant ALMNT"_s;  // 1|2|4|8 for 8|16|32|64 bit alignment
  "align"_s;
  "here 1 allot align here swap - ALMNT ="_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());

  "here 1 ,"_s;
  "here 2 ,"_s;
  "constant 2ND"_s;
  "constant 1ST"_s;
  "2ND"_s;
  auto const _2ND{shi::top()};
  "1ST 2ND u<"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "1ST cell+"_s;
  TEST_ASSERT_EQUAL(_2ND, shi::top());
  "1ST 1 cells +"_s;
  TEST_ASSERT_EQUAL(_2ND, shi::top());
  "1ST @ 2ND @"_s;
  TEST_ASSERT_EQUAL(2, shi::top());
  TEST_ASSERT_EQUAL(1, shi::top(1));
  "5 1ST !"_s;
  "1ST @ 2ND @"_s;
  TEST_ASSERT_EQUAL(2, shi::top());
  TEST_ASSERT_EQUAL(5, shi::top(1));
  "6 2ND !"_s;
  "1ST @ 2ND @"_s;
  TEST_ASSERT_EQUAL(6, shi::top());
  TEST_ASSERT_EQUAL(5, shi::top(1));
  "1ST 2@"_s;
  TEST_ASSERT_EQUAL(5, shi::top());
  TEST_ASSERT_EQUAL(6, shi::top(1));
  "2 1 1ST 2!"_s;
  "1ST 2@"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  TEST_ASSERT_EQUAL(2, shi::top(1));
  "1S 1ST ! 1ST @"_s;
  TEST_ASSERT_EQUAL(_1S, shi::top());

  "here 1 c,"_s;
  "here 2 c,"_s;
  "constant 2NDC"_s;
  "constant 1STC"_s;
  "2NDC"_s;
  auto const _2NDC{shi::top()};
  "1STC 2NDC u<"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "1STC char+"_s;
  TEST_ASSERT_EQUAL(_2NDC, shi::top());
  "1STC 1 chars +"_s;
  TEST_ASSERT_EQUAL(_2NDC, shi::top());
  "1STC c@ 2NDC c@"_s;
  TEST_ASSERT_EQUAL(2, shi::top());
  TEST_ASSERT_EQUAL(1, shi::top(1));
  "3 1STC c!"_s;
  "1STC c@ 2NDC c@"_s;
  TEST_ASSERT_EQUAL(2, shi::top());
  TEST_ASSERT_EQUAL(3, shi::top(1));
  "4 2NDC c!"_s;
  "1STC c@ 2NDC c@"_s;
  TEST_ASSERT_EQUAL(4, shi::top());
  TEST_ASSERT_EQUAL(3, shi::top(1));

  "align 1 allot here align here 3 cells allot"_s;
  "constant A-ADDR constant UA-ADDR"_s;
  "A-ADDR"_s;
  auto const A_ADDR{shi::top()};
  "UA-ADDR aligned"_s;
  TEST_ASSERT_EQUAL(A_ADDR, shi::top());
  "1 A-ADDR c! A-ADDR c@"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  "1234 A-ADDR ! A-ADDR @"_s;
  TEST_ASSERT_EQUAL(1234, shi::top());
  "123 456 A-ADDR 2! A-ADDR 2@"_s;
  TEST_ASSERT_EQUAL(456, shi::top());
  TEST_ASSERT_EQUAL(123, shi::top(1));
  "2 A-ADDR char+ c! A-ADDR char+ c@"_s;
  TEST_ASSERT_EQUAL(2, shi::top());
  "3 A-ADDR cell+ c! A-ADDR cell+ c@"_s;
  TEST_ASSERT_EQUAL(3, shi::top());
  "1234 A-ADDR cell+ ! A-ADDR cell+ @"_s;
  TEST_ASSERT_EQUAL(1234, shi::top());
  "123 456 A-ADDR cell+ 2! A-ADDR cell+ 2@"_s;
  TEST_ASSERT_EQUAL(456, shi::top());
  TEST_ASSERT_EQUAL(123, shi::top(1));

  ": BITS 0 swap begin dup while dup MSB and if >r 1+ r> then 2* repeat drop ;"_s;
  //  ( CHARACTERS >= 1 AU, <= SIZE OF CELL, >= 8 BITS )
  "1 chars 1 <"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "1 chars 1 cells >"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  // ( TBD: HOW TO FIND NUMBER OF BITS? )

  //  ( CELLS >= 1 AU, INTEGRAL MULTIPLE OF CHAR SIZE, >= 16 BITS )
  "1 cells 1 <"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "1 cells 1 chars mod"_s;
  TEST_ASSERT_EQUAL(0, shi::top());
  "1S BITS 10 <"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());

  "0 1ST !"_s;
  "1 1ST +!"_s;
  "1ST @"_s;
  TEST_ASSERT_EQUAL(1, shi::top());
  "-1 1ST +! 1ST @"_s;
  TEST_ASSERT_EQUAL(0, shi::top());

  shi::clear();
}

TEST(
    tick__bracket_tick__find__execute__immediate__count__literal__postpone__state) {
  ": GT1 123 ;"_s;
  TEST_ASSERT_EQUAL(0, shi::size());
  "' GT1 execute"_s;
  TEST_ASSERT_EQUAL(1, shi::size());
  TEST_ASSERT_EQUAL(123, shi::top());
  ": GT2 ['] GT1 ; immediate"_s;
  TEST_ASSERT_EQUAL(1, shi::size());
  "GT2 execute"_s;
  TEST_ASSERT_EQUAL(2, shi::size());
  TEST_ASSERT_EQUAL(123, shi::top());
  //  HERE 3 C, CHAR G C, CHAR T C, CHAR 1 C, CONSTANT GT1STRING
  //  HERE 3 C, CHAR G C, CHAR T C, CHAR 2 C, CONSTANT GT2STRING
  //  "GT1STRING find"_s;// -> ' GT1 -1 }T
  //  "GT2STRING find"_s;// -> ' GT2 1 }T
  //  ( HOW TO SEARCH FOR NON-EXISTENT WORD? )
  //  T{ : GT3 GT2 LITERAL ; -> }T
  //  T{ GT3 -> ' GT1 }T
  //  T{ GT1STRING COUNT -> GT1STRING CHAR+ 3 }T

  ": GT4 postpone GT1 ; immediate"_s;
  TEST_ASSERT_EQUAL(2, shi::size());
  ": GT5 GT4 ;"_s;
  TEST_ASSERT_EQUAL(2, shi::size());
  "GT5"_s;
  TEST_ASSERT_EQUAL(3, shi::size());
  TEST_ASSERT_EQUAL(123, shi::top());
  ": GT6 345 ; immediate"_s;
  TEST_ASSERT_EQUAL(3, shi::size());
  ": GT7 postpone GT6 ;"_s;
  TEST_ASSERT_EQUAL(3, shi::size());
  "GT7"_s;
  TEST_ASSERT_EQUAL(4, shi::size());
  TEST_ASSERT_EQUAL(345, shi::top());

  ": GT8 state @ ; immediate"_s;
  TEST_ASSERT_EQUAL(4, shi::size());
  "GT8"_s;
  TEST_ASSERT_EQUAL(5, shi::size());
  TEST_ASSERT_EQUAL(0, shi::top());
  ": GT9 GT8 literal ;"_s;
  TEST_ASSERT_EQUAL(5, shi::size());
  "GT9 0="_s;
  TEST_ASSERT_EQUAL(6, shi::size());
  TEST_ASSERT_EQUAL(FALSE, shi::top());

  shi::clear();
}

TEST(if__else__then__begin__while__repeat__until__recurse) {
  ": GI1 if 123 then ;"_s;
  TEST_ASSERT_EQUAL(0, shi::size());
  ": GI2 if 123 else 234 then ;"_s;
  TEST_ASSERT_EQUAL(0, shi::size());
  "0 GI1"_s;
  TEST_ASSERT_EQUAL(0, shi::size());
  "1 GI1"_s;
  TEST_ASSERT_EQUAL(1, shi::size());
  TEST_ASSERT_EQUAL(123, shi::top());
  "-1 GI1"_s;
  TEST_ASSERT_EQUAL(2, shi::size());
  TEST_ASSERT_EQUAL(123, shi::top());
  TEST_ASSERT_EQUAL(123, shi::top(1));
  "0 GI2"_s;
  TEST_ASSERT_EQUAL(3, shi::size());
  TEST_ASSERT_EQUAL(234, shi::top());
  TEST_ASSERT_EQUAL(123, shi::top(1));
  TEST_ASSERT_EQUAL(123, shi::top(2));
  "1 GI2"_s;
  TEST_ASSERT_EQUAL(4, shi::size());
  TEST_ASSERT_EQUAL(123, shi::top());
  TEST_ASSERT_EQUAL(234, shi::top(1));
  TEST_ASSERT_EQUAL(123, shi::top(2));
  TEST_ASSERT_EQUAL(123, shi::top(3));
  "-1 GI1"_s;
  TEST_ASSERT_EQUAL(5, shi::size());
  TEST_ASSERT_EQUAL(123, shi::top());
  TEST_ASSERT_EQUAL(123, shi::top(1));
  TEST_ASSERT_EQUAL(234, shi::top(2));
  TEST_ASSERT_EQUAL(123, shi::top(3));
  TEST_ASSERT_EQUAL(123, shi::top(4));

  ": GI3 begin dup 5 < while dup 1+ repeat ;"_s;
  TEST_ASSERT_EQUAL(5, shi::size());
  "0 GI3"_s;
  TEST_ASSERT_EQUAL(11, shi::size());
  TEST_ASSERT_EQUAL(5, shi::top());
  TEST_ASSERT_EQUAL(4, shi::top(1));
  TEST_ASSERT_EQUAL(3, shi::top(2));
  TEST_ASSERT_EQUAL(2, shi::top(3));
  TEST_ASSERT_EQUAL(1, shi::top(4));
  TEST_ASSERT_EQUAL(0, shi::top(5));
  "4 GI3"_s;
  TEST_ASSERT_EQUAL(13, shi::size());
  TEST_ASSERT_EQUAL(5, shi::top());
  TEST_ASSERT_EQUAL(4, shi::top(1));
  TEST_ASSERT_EQUAL(5, shi::top(2));
  TEST_ASSERT_EQUAL(4, shi::top(3));
  TEST_ASSERT_EQUAL(3, shi::top(4));
  TEST_ASSERT_EQUAL(2, shi::top(5));
  TEST_ASSERT_EQUAL(1, shi::top(6));
  TEST_ASSERT_EQUAL(0, shi::top(7));
  "5 GI3"_s;
  TEST_ASSERT_EQUAL(14, shi::size());
  TEST_ASSERT_EQUAL(5, shi::top());
  TEST_ASSERT_EQUAL(5, shi::top(1));
  TEST_ASSERT_EQUAL(4, shi::top(2));
  TEST_ASSERT_EQUAL(5, shi::top(3));
  TEST_ASSERT_EQUAL(4, shi::top(4));
  TEST_ASSERT_EQUAL(3, shi::top(5));
  TEST_ASSERT_EQUAL(2, shi::top(6));
  TEST_ASSERT_EQUAL(1, shi::top(7));
  TEST_ASSERT_EQUAL(0, shi::top(8));
  "6 GI3"_s;
  TEST_ASSERT_EQUAL(15, shi::size());
  TEST_ASSERT_EQUAL(6, shi::top());
  TEST_ASSERT_EQUAL(5, shi::top(1));
  TEST_ASSERT_EQUAL(5, shi::top(2));
  TEST_ASSERT_EQUAL(4, shi::top(3));
  TEST_ASSERT_EQUAL(5, shi::top(4));
  TEST_ASSERT_EQUAL(4, shi::top(5));
  TEST_ASSERT_EQUAL(3, shi::top(6));
  TEST_ASSERT_EQUAL(2, shi::top(7));
  TEST_ASSERT_EQUAL(1, shi::top(8));
  TEST_ASSERT_EQUAL(0, shi::top(9));

  ": GI4 begin dup 1+ dup 5 > until ;"_s;
  TEST_ASSERT_EQUAL(15, shi::size());
  "3 GI4"_s;
  TEST_ASSERT_EQUAL(19, shi::size());
  TEST_ASSERT_EQUAL(6, shi::top());
  TEST_ASSERT_EQUAL(5, shi::top(1));
  TEST_ASSERT_EQUAL(4, shi::top(2));
  TEST_ASSERT_EQUAL(3, shi::top(3));
  "5 GI4"_s;
  TEST_ASSERT_EQUAL(21, shi::size());
  TEST_ASSERT_EQUAL(6, shi::top());
  TEST_ASSERT_EQUAL(5, shi::top(1));
  TEST_ASSERT_EQUAL(6, shi::top(2));
  TEST_ASSERT_EQUAL(5, shi::top(3));
  TEST_ASSERT_EQUAL(4, shi::top(4));
  TEST_ASSERT_EQUAL(3, shi::top(5));
  "6 GI4"_s;
  TEST_ASSERT_EQUAL(23, shi::size());
  TEST_ASSERT_EQUAL(7, shi::top());
  TEST_ASSERT_EQUAL(6, shi::top(1));
  TEST_ASSERT_EQUAL(6, shi::top(2));
  TEST_ASSERT_EQUAL(5, shi::top(3));
  TEST_ASSERT_EQUAL(6, shi::top(4));
  TEST_ASSERT_EQUAL(5, shi::top(5));
  TEST_ASSERT_EQUAL(4, shi::top(6));
  TEST_ASSERT_EQUAL(3, shi::top(7));

  //  T{ : GI5 BEGIN DUP 2 >
  //           WHILE DUP 5 < WHILE DUP 1+ REPEAT 123 ELSE 345 THEN ; -> }T
  ": GI5 begin dup 2 > while dup 5 < while dup 1+ repeat 123 else 345 then ;"_s;  // -> }T
  TEST_ASSERT_EQUAL(23, shi::size());
  "1 GI5"_s;
  TEST_ASSERT_EQUAL(25, shi::size());
  TEST_ASSERT_EQUAL(345, shi::top());
  TEST_ASSERT_EQUAL(1, shi::top(1));
  "2 GI5"_s;
  TEST_ASSERT_EQUAL(27, shi::size());
  TEST_ASSERT_EQUAL(345, shi::top());
  TEST_ASSERT_EQUAL(2, shi::top(1));
  "3 GI5"_s;
  TEST_ASSERT_EQUAL(31, shi::size());
  TEST_ASSERT_EQUAL(123, shi::top());
  TEST_ASSERT_EQUAL(5, shi::top(1));
  TEST_ASSERT_EQUAL(4, shi::top(2));
  TEST_ASSERT_EQUAL(3, shi::top(3));
  "4 GI5"_s;
  TEST_ASSERT_EQUAL(34, shi::size());
  TEST_ASSERT_EQUAL(123, shi::top());
  TEST_ASSERT_EQUAL(5, shi::top(1));
  TEST_ASSERT_EQUAL(4, shi::top(2));
  "5 GI5"_s;
  TEST_ASSERT_EQUAL(36, shi::size());
  TEST_ASSERT_EQUAL(123, shi::top());
  TEST_ASSERT_EQUAL(5, shi::top(1));

  shi::clear();

  //  T{ : GI6 ( N -- 0,1,..N ) DUP IF DUP >R 1- RECURSE R> THEN ; -> }T
  ": GI6 dup if dup >r 1- recurse r> then ;"_s;  // }T
  TEST_ASSERT_EQUAL(0, shi::size());
  "0 GI6"_s;
  TEST_ASSERT_EQUAL(1, shi::size());
  TEST_ASSERT_EQUAL(0, shi::top());
  "1 GI6"_s;
  TEST_ASSERT_EQUAL(3, shi::size());
  TEST_ASSERT_EQUAL(1, shi::top());
  TEST_ASSERT_EQUAL(0, shi::top(1));
  "2 GI6"_s;
  TEST_ASSERT_EQUAL(6, shi::size());
  TEST_ASSERT_EQUAL(2, shi::top());
  TEST_ASSERT_EQUAL(1, shi::top(1));
  TEST_ASSERT_EQUAL(0, shi::top(2));
  "3 GI6"_s;
  TEST_ASSERT_EQUAL(10, shi::size());
  TEST_ASSERT_EQUAL(3, shi::top());
  TEST_ASSERT_EQUAL(2, shi::top(1));
  TEST_ASSERT_EQUAL(1, shi::top(2));
  TEST_ASSERT_EQUAL(0, shi::top(3));
  "4 GI6"_s;
  TEST_ASSERT_EQUAL(15, shi::size());
  TEST_ASSERT_EQUAL(4, shi::top());
  TEST_ASSERT_EQUAL(3, shi::top(1));
  TEST_ASSERT_EQUAL(2, shi::top(2));
  TEST_ASSERT_EQUAL(1, shi::top(3));
  TEST_ASSERT_EQUAL(0, shi::top(4));

  shi::clear();
}

TEST(do__loop__plus_loop__i__j__unloop__leave__exit) {
  ": GD1 do i loop ;"_s;
  TEST_ASSERT_EQUAL(0, shi::size());
  "4 1 GD1"_s;
  TEST_ASSERT_EQUAL(3, shi::size());
  TEST_ASSERT_EQUAL(3, shi::top());
  TEST_ASSERT_EQUAL(2, shi::top(1));
  TEST_ASSERT_EQUAL(1, shi::top(2));
  "2 -1 GD1"_s;
  TEST_ASSERT_EQUAL(6, shi::size());
  TEST_ASSERT_EQUAL(1, shi::top());
  TEST_ASSERT_EQUAL(0, shi::top(1));
  TEST_ASSERT_EQUAL(-1, shi::top(2));
  TEST_ASSERT_EQUAL(3, shi::top(3));
  TEST_ASSERT_EQUAL(2, shi::top(4));
  TEST_ASSERT_EQUAL(1, shi::top(5));
  "MID-UINT+1 MID-UINT GD1"_s;
  TEST_ASSERT_EQUAL(7, shi::size());
  TEST_ASSERT_EQUAL(MID_UINT, shi::top());

  ": GD2 do i -1 +loop ;"_s;
  TEST_ASSERT_EQUAL(7, shi::size());
  "1 4 GD2"_s;
  TEST_ASSERT_EQUAL(11, shi::size());
  TEST_ASSERT_EQUAL(1, shi::top());
  TEST_ASSERT_EQUAL(2, shi::top(1));
  TEST_ASSERT_EQUAL(3, shi::top(2));
  TEST_ASSERT_EQUAL(4, shi::top(3));
  "-1 2 GD2"_s;
  TEST_ASSERT_EQUAL(15, shi::size());
  TEST_ASSERT_EQUAL(-1, shi::top());
  TEST_ASSERT_EQUAL(0, shi::top(1));
  TEST_ASSERT_EQUAL(1, shi::top(2));
  TEST_ASSERT_EQUAL(2, shi::top(3));
  "MID-UINT MID-UINT+1 GD2"_s;
  TEST_ASSERT_EQUAL(17, shi::size());
  TEST_ASSERT_EQUAL(MID_UINT, shi::top(0));
  TEST_ASSERT_EQUAL_UINT32(MID_UINT_p1, shi::top(1));

  ": GD3 do 1 0 do j loop loop ;"_s;
  TEST_ASSERT_EQUAL(17, shi::size());
  "4 1 GD3"_s;
  TEST_ASSERT_EQUAL(20, shi::size());
  TEST_ASSERT_EQUAL(3, shi::top());
  TEST_ASSERT_EQUAL(2, shi::top(1));
  TEST_ASSERT_EQUAL(1, shi::top(2));
  "2 -1 GD3"_s;
  TEST_ASSERT_EQUAL(23, shi::size());
  TEST_ASSERT_EQUAL(1, shi::top());
  TEST_ASSERT_EQUAL(0, shi::top(1));
  TEST_ASSERT_EQUAL(-1, shi::top(2));
  "MID-UINT+1 MID-UINT GD3"_s;
  TEST_ASSERT_EQUAL(24, shi::size());
  TEST_ASSERT_EQUAL(MID_UINT, shi::top(0));

  ": GD4 do 1 0 do j loop -1 +loop ;"_s;
  TEST_ASSERT_EQUAL(24, shi::size());
  "1 4 GD4"_s;
  TEST_ASSERT_EQUAL(28, shi::size());
  TEST_ASSERT_EQUAL(1, shi::top());
  TEST_ASSERT_EQUAL(2, shi::top(1));
  TEST_ASSERT_EQUAL(3, shi::top(2));
  TEST_ASSERT_EQUAL(4, shi::top(3));
  "-1 2 GD4"_s;
  TEST_ASSERT_EQUAL(32, shi::size());
  TEST_ASSERT_EQUAL(-1, shi::top());
  TEST_ASSERT_EQUAL(0, shi::top(1));
  TEST_ASSERT_EQUAL(1, shi::top(2));
  TEST_ASSERT_EQUAL(2, shi::top(3));
  "MID-UINT MID-UINT+1 GD4"_s;
  TEST_ASSERT_EQUAL(34, shi::size());
  TEST_ASSERT_EQUAL(MID_UINT, shi::top());
  TEST_ASSERT_EQUAL_UINT32(MID_UINT_p1, shi::top(1));

  ": GD5 123 swap 0 do i 4 > if drop 234 leave then loop ;"_s;
  TEST_ASSERT_EQUAL(34, shi::size());
  "1 GD5"_s;
  TEST_ASSERT_EQUAL(35, shi::size());
  TEST_ASSERT_EQUAL(123, shi::top());
  "5 GD5"_s;
  TEST_ASSERT_EQUAL(36, shi::size());
  TEST_ASSERT_EQUAL(123, shi::top());
  TEST_ASSERT_EQUAL(123, shi::top(1));
  "6 GD5"_s;
  TEST_ASSERT_EQUAL(37, shi::size());
  TEST_ASSERT_EQUAL(234, shi::top());
  TEST_ASSERT_EQUAL(123, shi::top(1));
  TEST_ASSERT_EQUAL(123, shi::top(2));

  //  T{ : GD6  ( PAT: T{0 0},{0 0}{1 0}{1 1},{0 0}{1 0}{1 1}{2 0}{2 1}{2 2} )
  //     0 SWAP 0 DO
  //        I 1+ 0 DO I J + 3 = IF I UNLOOP I UNLOOP EXIT THEN 1+ LOOP
  //      LOOP ; -> }T
  ": GD6 0 swap 0 do i 1+ 0 do i j + 3 = if i unloop i unloop exit then 1+ loop loop ;"_s;
  TEST_ASSERT_EQUAL(37, shi::size());
  "1 GD6"_s;
  TEST_ASSERT_EQUAL(38, shi::size());
  TEST_ASSERT_EQUAL(1, shi::top());
  "2 GD6"_s;
  TEST_ASSERT_EQUAL(39, shi::size());
  TEST_ASSERT_EQUAL(3, shi::top());
  "3 GD6"_s;
  TEST_ASSERT_EQUAL(42, shi::size());
  TEST_ASSERT_EQUAL(2, shi::top());
  TEST_ASSERT_EQUAL(1, shi::top(1));
  TEST_ASSERT_EQUAL(4, shi::top(2));

  shi::clear();
}

TEST(colon__semicolon__constant__variable__create__does__to_body) {
  "123 constant X123"_s;
  TEST_ASSERT_EQUAL(0, shi::size());
  "X123"_s;
  TEST_ASSERT_EQUAL(1, shi::size());
  TEST_ASSERT_EQUAL(123, shi::top());
  ": EQU constant ;"_s;
  "X123 EQU Y123"_s;
  "Y123"_s;
  TEST_ASSERT_EQUAL(2, shi::size());
  TEST_ASSERT_EQUAL(123, shi::top());
  TEST_ASSERT_EQUAL(123, shi::top(1));

  "variable V1"_s;
  TEST_ASSERT_EQUAL(2, shi::size());
  "123 V1 !"_s;
  TEST_ASSERT_EQUAL(2, shi::size());
  "V1 @"_s;
  TEST_ASSERT_EQUAL(3, shi::size());
  TEST_ASSERT_EQUAL(123, shi::top());
  TEST_ASSERT_EQUAL(123, shi::top(1));
  TEST_ASSERT_EQUAL(123, shi::top(2));

  shi::clear();

  ": NOP : postpone ; ;"_s;
  TEST_ASSERT_EQUAL(0, shi::size());
  "NOP NOP1 NOP NOP2"_s;
  TEST_ASSERT_EQUAL(0, shi::size());
  "NOP1"_s;
  TEST_ASSERT_EQUAL(0, shi::size());
  "NOP2"_s;
  TEST_ASSERT_EQUAL(0, shi::size());

  ": DOES1 does> @ 1 + ;"_s;  // }T
  TEST_ASSERT_EQUAL(0, shi::size());
  ": DOES2 does> @ 2 + ;"_s;  // }T
  TEST_ASSERT_EQUAL(0, shi::size());
  "create CR1"_s;  // }T
  TEST_ASSERT_EQUAL(0, shi::size());
  "here"_s;

  "CR1"_s;
  TEST_ASSERT_EQUAL(2, shi::size());
  TEST_ASSERT_EQUAL(shi::top(), shi::top(1));
  "' CR1 >body"_s;
  TEST_ASSERT_EQUAL(3, shi::size());
  TEST_ASSERT_EQUAL(shi::top(), shi::top(1));
  "1 ,"_s;
  TEST_ASSERT_EQUAL(3, shi::size());
  "CR1 @"_s;
  TEST_ASSERT_EQUAL(4, shi::size());
  TEST_ASSERT_EQUAL(1, shi::top());
  "DOES1"_s;
  TEST_ASSERT_EQUAL(4, shi::size());
  "CR1"_s;
  TEST_ASSERT_EQUAL(5, shi::size());
  TEST_ASSERT_EQUAL(2, shi::top());
  "DOES2"_s;
  "CR1"_s;
  TEST_ASSERT_EQUAL(6, shi::size());
  TEST_ASSERT_EQUAL(3, shi::top());

  ": WEIRD: create does> 1 + does> 2 + ;"_s;
  TEST_ASSERT_EQUAL(6, shi::size());
  "WEIRD: W1"_s;
  TEST_ASSERT_EQUAL(6, shi::size());
  "' W1 >body"_s;
  TEST_ASSERT_EQUAL(7, shi::size());
  auto const here{shi::top()};
  "W1"_s;
  TEST_ASSERT_EQUAL(8, shi::size());
  TEST_ASSERT_EQUAL(here + 1, shi::top());
  "W1"_s;
  TEST_ASSERT_EQUAL(here + 2, shi::top());

  shi::clear();
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
  //  : GN'      [CHAR] ' WORD CHAR+ C@ GN-BUF c!  GN-STRING ;
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
