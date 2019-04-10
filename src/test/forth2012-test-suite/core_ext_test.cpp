#include "forth2012_test_suite.hpp"

using namespace shi::literals;

TEST(ne__u_more) {
  "0 0 <>"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "1 1 <>"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "-1 -1 <>"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "1 0 <>"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "-1 0 <>"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "0 1 <>"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "0 -1 <>"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());

  "0 1 u>"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "1 2 u>"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "0 MID-UINT u>"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "0 MAX-UINT u>"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "MID-UINT MAX-UINT u>"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "0 0 u>"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "1 1 u>"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "1 0 u>"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "2 1 u>"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "MID-UINT 0 u>"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "MAX-UINT 0 u>"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "MAX-UINT MID-UINT u>"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());

  shi::clear();
}

TEST(zero_ne__zero_more) {
  "0 0<>"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "1 0<>"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "2 0<>"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "-1 0<>"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "MAX-UINT 0<>"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "MIN-INT 0<>"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "MAX-INT 0<>"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());

  "0 0>"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "-1 0>"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "MIN-INT 0>"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  "1 0>"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  "MAX-INT 0>"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());

  shi::clear();
}

TEST(nip__tuck__roll__pick) {
  "1 2 nip"_s;
  TEST_ASSERT_EQUAL(2, shi::top());
  "1 2 3 nip"_s;
  TEST_ASSERT_EQUAL(3, shi::top());
  TEST_ASSERT_EQUAL(1, shi::top(1));
  TEST_ASSERT_EQUAL(2, shi::top(2));

  "1 2 tuck"_s;
  TEST_ASSERT_EQUAL(2, shi::top());
  TEST_ASSERT_EQUAL(1, shi::top(1));
  TEST_ASSERT_EQUAL(2, shi::top(2));
  TEST_ASSERT_EQUAL(3, shi::top(3));
  TEST_ASSERT_EQUAL(1, shi::top(4));
  TEST_ASSERT_EQUAL(2, shi::top(5));
  "1 2 3 tuck"_s;
  TEST_ASSERT_EQUAL(3, shi::top());
  TEST_ASSERT_EQUAL(2, shi::top(1));
  TEST_ASSERT_EQUAL(3, shi::top(2));
  TEST_ASSERT_EQUAL(1, shi::top(3));
  TEST_ASSERT_EQUAL(2, shi::top(4));
  TEST_ASSERT_EQUAL(1, shi::top(5));
  TEST_ASSERT_EQUAL(2, shi::top(6));
  TEST_ASSERT_EQUAL(3, shi::top(7));
  TEST_ASSERT_EQUAL(1, shi::top(8));
  TEST_ASSERT_EQUAL(2, shi::top(9));

  shi::clear();

  ": RO5 100 200 300 400 500 ;"_s;
  "RO5 3 roll"_s;
  TEST_ASSERT_EQUAL(200, shi::top());
  TEST_ASSERT_EQUAL(500, shi::top(1));
  TEST_ASSERT_EQUAL(400, shi::top(2));
  TEST_ASSERT_EQUAL(300, shi::top(3));
  TEST_ASSERT_EQUAL(100, shi::top(4));
  "RO5 2 roll"_s;
  TEST_ASSERT_EQUAL(300, shi::top());
  TEST_ASSERT_EQUAL(500, shi::top(1));
  TEST_ASSERT_EQUAL(400, shi::top(2));
  TEST_ASSERT_EQUAL(200, shi::top(3));
  TEST_ASSERT_EQUAL(100, shi::top(4));
  "RO5 1 roll"_s;
  TEST_ASSERT_EQUAL(400, shi::top());
  TEST_ASSERT_EQUAL(500, shi::top(1));
  TEST_ASSERT_EQUAL(300, shi::top(2));
  TEST_ASSERT_EQUAL(200, shi::top(3));
  TEST_ASSERT_EQUAL(100, shi::top(4));
  "RO5 0 roll"_s;
  TEST_ASSERT_EQUAL(500, shi::top());
  TEST_ASSERT_EQUAL(400, shi::top(1));
  TEST_ASSERT_EQUAL(300, shi::top(2));
  TEST_ASSERT_EQUAL(200, shi::top(3));
  TEST_ASSERT_EQUAL(100, shi::top(4));

  shi::clear();

  "RO5 2 pick"_s;
  TEST_ASSERT_EQUAL(300, shi::top());
  TEST_ASSERT_EQUAL(500, shi::top(1));
  TEST_ASSERT_EQUAL(400, shi::top(2));
  TEST_ASSERT_EQUAL(300, shi::top(3));
  TEST_ASSERT_EQUAL(200, shi::top(4));
  TEST_ASSERT_EQUAL(100, shi::top(5));
  "RO5 1 pick"_s;
  TEST_ASSERT_EQUAL(400, shi::top());
  TEST_ASSERT_EQUAL(500, shi::top(1));
  TEST_ASSERT_EQUAL(400, shi::top(2));
  TEST_ASSERT_EQUAL(300, shi::top(3));
  TEST_ASSERT_EQUAL(200, shi::top(4));
  TEST_ASSERT_EQUAL(100, shi::top(5));
  "RO5 0 pick"_s;
  TEST_ASSERT_EQUAL(500, shi::top());
  TEST_ASSERT_EQUAL(500, shi::top(1));
  TEST_ASSERT_EQUAL(400, shi::top(2));
  TEST_ASSERT_EQUAL(300, shi::top(3));
  TEST_ASSERT_EQUAL(200, shi::top(4));
  TEST_ASSERT_EQUAL(100, shi::top(5));

  shi::clear();
}

TEST(two_to_r__two_r_fetch__two_r_from) {
  //  T{ : RR0 2>R 100 R> R> ; -> }T
  //  T{ 300 400 RR0 -> 100 400 300 }T
  //  T{ 200 300 400 RR0 -> 200 100 400 300 }T
  //
  //  T{ : RR1 2>R 100 2R@ R> R> ; -> }T
  //  T{ 300 400 RR1 -> 100 300 400 400 300 }T
  //  T{ 200 300 400 RR1 -> 200 100 300 400 400 300 }T
  //
  //  T{ : RR2 2>R 100 2R> ; -> }T
  //  T{ 300 400 RR2 -> 100 300 400 }T
  //  T{ 200 300 400 RR2 -> 200 100 300 400 }T
}

TEST(hex) {
  "base @ hex base @ decimal base @ - swap base !"_s;
  TEST_ASSERT_EQUAL(1, shi::size());
  TEST_ASSERT_EQUAL(6, shi::top());

  shi::clear();
}

TEST(within) {
  //  T{ 0 0 0 WITHIN -> FALSE }T
  //  T{ 0 0 MID-UINT WITHIN -> TRUE }T
  //  T{ 0 0 MID-UINT+1 WITHIN -> TRUE }T
  //  T{ 0 0 MAX-UINT WITHIN -> TRUE }T
  //  T{ 0 MID-UINT 0 WITHIN -> FALSE }T
  //  T{ 0 MID-UINT MID-UINT WITHIN -> FALSE }T
  //  T{ 0 MID-UINT MID-UINT+1 WITHIN -> FALSE }T
  //  T{ 0 MID-UINT MAX-UINT WITHIN -> FALSE }T
  //  T{ 0 MID-UINT+1 0 WITHIN -> FALSE }T
  //  T{ 0 MID-UINT+1 MID-UINT WITHIN -> TRUE }T
  //  T{ 0 MID-UINT+1 MID-UINT+1 WITHIN -> FALSE }T
  //  T{ 0 MID-UINT+1 MAX-UINT WITHIN -> FALSE }T
  //  T{ 0 MAX-UINT 0 WITHIN -> FALSE }T
  //  T{ 0 MAX-UINT MID-UINT WITHIN -> TRUE }T
  //  T{ 0 MAX-UINT MID-UINT+1 WITHIN -> TRUE }T
  //  T{ 0 MAX-UINT MAX-UINT WITHIN -> FALSE }T
  //  T{ MID-UINT 0 0 WITHIN -> FALSE }T
  //  T{ MID-UINT 0 MID-UINT WITHIN -> FALSE }T
  //  T{ MID-UINT 0 MID-UINT+1 WITHIN -> TRUE }T
  //  T{ MID-UINT 0 MAX-UINT WITHIN -> TRUE }T
  //  T{ MID-UINT MID-UINT 0 WITHIN -> TRUE }T
  //  T{ MID-UINT MID-UINT MID-UINT WITHIN -> FALSE }T
  //  T{ MID-UINT MID-UINT MID-UINT+1 WITHIN -> TRUE }T
  //  T{ MID-UINT MID-UINT MAX-UINT WITHIN -> TRUE }T
  //  T{ MID-UINT MID-UINT+1 0 WITHIN -> FALSE }T
  //  T{ MID-UINT MID-UINT+1 MID-UINT WITHIN -> FALSE }T
  //  T{ MID-UINT MID-UINT+1 MID-UINT+1 WITHIN -> FALSE }T
  //  T{ MID-UINT MID-UINT+1 MAX-UINT WITHIN -> FALSE }T
  //  T{ MID-UINT MAX-UINT 0 WITHIN -> FALSE }T
  //  T{ MID-UINT MAX-UINT MID-UINT WITHIN -> FALSE }T
  //  T{ MID-UINT MAX-UINT MID-UINT+1 WITHIN -> TRUE }T
  //  T{ MID-UINT MAX-UINT MAX-UINT WITHIN -> FALSE }T
  //  T{ MID-UINT+1 0 0 WITHIN -> FALSE }T
  //  T{ MID-UINT+1 0 MID-UINT WITHIN -> FALSE }T
  //  T{ MID-UINT+1 0 MID-UINT+1 WITHIN -> FALSE }T
  //  T{ MID-UINT+1 0 MAX-UINT WITHIN -> TRUE }T
  //  T{ MID-UINT+1 MID-UINT 0 WITHIN -> TRUE }T
  //  T{ MID-UINT+1 MID-UINT MID-UINT WITHIN -> FALSE }T
  //  T{ MID-UINT+1 MID-UINT MID-UINT+1 WITHIN -> FALSE }T
  //  T{ MID-UINT+1 MID-UINT MAX-UINT WITHIN -> TRUE }T
  //  T{ MID-UINT+1 MID-UINT+1 0 WITHIN -> TRUE }T
  //  T{ MID-UINT+1 MID-UINT+1 MID-UINT WITHIN -> TRUE }T
  //  T{ MID-UINT+1 MID-UINT+1 MID-UINT+1 WITHIN -> FALSE }T
  //  T{ MID-UINT+1 MID-UINT+1 MAX-UINT WITHIN -> TRUE }T
  //  T{ MID-UINT+1 MAX-UINT 0 WITHIN -> FALSE }T
  //  T{ MID-UINT+1 MAX-UINT MID-UINT WITHIN -> FALSE }T
  //  T{ MID-UINT+1 MAX-UINT MID-UINT+1 WITHIN -> FALSE }T
  //  T{ MID-UINT+1 MAX-UINT MAX-UINT WITHIN -> FALSE }T
  //  T{ MAX-UINT 0 0 WITHIN -> FALSE }T
  //  T{ MAX-UINT 0 MID-UINT WITHIN -> FALSE }T
  //  T{ MAX-UINT 0 MID-UINT+1 WITHIN -> FALSE }T
  //  T{ MAX-UINT 0 MAX-UINT WITHIN -> FALSE }T
  //  T{ MAX-UINT MID-UINT 0 WITHIN -> TRUE }T
  //  T{ MAX-UINT MID-UINT MID-UINT WITHIN -> FALSE }T
  //  T{ MAX-UINT MID-UINT MID-UINT+1 WITHIN -> FALSE }T
  //  T{ MAX-UINT MID-UINT MAX-UINT WITHIN -> FALSE }T
  //  T{ MAX-UINT MID-UINT+1 0 WITHIN -> TRUE }T
  //  T{ MAX-UINT MID-UINT+1 MID-UINT WITHIN -> TRUE }T
  //  T{ MAX-UINT MID-UINT+1 MID-UINT+1 WITHIN -> FALSE }T
  //  T{ MAX-UINT MID-UINT+1 MAX-UINT WITHIN -> FALSE }T
  //  T{ MAX-UINT MAX-UINT 0 WITHIN -> TRUE }T
  //  T{ MAX-UINT MAX-UINT MID-UINT WITHIN -> TRUE }T
  //  T{ MAX-UINT MAX-UINT MID-UINT+1 WITHIN -> TRUE }T
  //  T{ MAX-UINT MAX-UINT MAX-UINT WITHIN -> FALSE }T
  //
  //  T{ MIN-INT MIN-INT MIN-INT WITHIN -> FALSE }T
  //  T{ MIN-INT MIN-INT 0 WITHIN -> TRUE }T
  //  T{ MIN-INT MIN-INT 1 WITHIN -> TRUE }T
  //  T{ MIN-INT MIN-INT MAX-INT WITHIN -> TRUE }T
  //  T{ MIN-INT 0 MIN-INT WITHIN -> FALSE }T
  //  T{ MIN-INT 0 0 WITHIN -> FALSE }T
  //  T{ MIN-INT 0 1 WITHIN -> FALSE }T
  //  T{ MIN-INT 0 MAX-INT WITHIN -> FALSE }T
  //  T{ MIN-INT 1 MIN-INT WITHIN -> FALSE }T
  //  T{ MIN-INT 1 0 WITHIN -> TRUE }T
  //  T{ MIN-INT 1 1 WITHIN -> FALSE }T
  //  T{ MIN-INT 1 MAX-INT WITHIN -> FALSE }T
  //  T{ MIN-INT MAX-INT MIN-INT WITHIN -> FALSE }T
  //  T{ MIN-INT MAX-INT 0 WITHIN -> TRUE }T
  //  T{ MIN-INT MAX-INT 1 WITHIN -> TRUE }T
  //  T{ MIN-INT MAX-INT MAX-INT WITHIN -> FALSE }T
  //  T{ 0 MIN-INT MIN-INT WITHIN -> FALSE }T
  //  T{ 0 MIN-INT 0 WITHIN -> FALSE }T
  //  T{ 0 MIN-INT 1 WITHIN -> TRUE }T
  //  T{ 0 MIN-INT MAX-INT WITHIN -> TRUE }T
  //  T{ 0 0 MIN-INT WITHIN -> TRUE }T
  //  T{ 0 0 0 WITHIN -> FALSE }T
  //  T{ 0 0 1 WITHIN -> TRUE }T
  //  T{ 0 0 MAX-INT WITHIN -> TRUE }T
  //  T{ 0 1 MIN-INT WITHIN -> FALSE }T
  //  T{ 0 1 0 WITHIN -> FALSE }T
  //  T{ 0 1 1 WITHIN -> FALSE }T
  //  T{ 0 1 MAX-INT WITHIN -> FALSE }T
  //  T{ 0 MAX-INT MIN-INT WITHIN -> FALSE }T
  //  T{ 0 MAX-INT 0 WITHIN -> FALSE }T
  //  T{ 0 MAX-INT 1 WITHIN -> TRUE }T
  //  T{ 0 MAX-INT MAX-INT WITHIN -> FALSE }T
  //  T{ 1 MIN-INT MIN-INT WITHIN -> FALSE }T
  //  T{ 1 MIN-INT 0 WITHIN -> FALSE }T
  //  T{ 1 MIN-INT 1 WITHIN -> FALSE }T
  //  T{ 1 MIN-INT MAX-INT WITHIN -> TRUE }T
  //  T{ 1 0 MIN-INT WITHIN -> TRUE }T
  //  T{ 1 0 0 WITHIN -> FALSE }T
  //  T{ 1 0 1 WITHIN -> FALSE }T
  //  T{ 1 0 MAX-INT WITHIN -> TRUE }T
  //  T{ 1 1 MIN-INT WITHIN -> TRUE }T
  //  T{ 1 1 0 WITHIN -> TRUE }T
  //  T{ 1 1 1 WITHIN -> FALSE }T
  //  T{ 1 1 MAX-INT WITHIN -> TRUE }T
  //  T{ 1 MAX-INT MIN-INT WITHIN -> FALSE }T
  //  T{ 1 MAX-INT 0 WITHIN -> FALSE }T
  //  T{ 1 MAX-INT 1 WITHIN -> FALSE }T
  //  T{ 1 MAX-INT MAX-INT WITHIN -> FALSE }T
  //  T{ MAX-INT MIN-INT MIN-INT WITHIN -> FALSE }T
  //  T{ MAX-INT MIN-INT 0 WITHIN -> FALSE }T
  //  T{ MAX-INT MIN-INT 1 WITHIN -> FALSE }T
  //  T{ MAX-INT MIN-INT MAX-INT WITHIN -> FALSE }T
  //  T{ MAX-INT 0 MIN-INT WITHIN -> TRUE }T
  //  T{ MAX-INT 0 0 WITHIN -> FALSE }T
  //  T{ MAX-INT 0 1 WITHIN -> FALSE }T
  //  T{ MAX-INT 0 MAX-INT WITHIN -> FALSE }T
  //  T{ MAX-INT 1 MIN-INT WITHIN -> TRUE }T
  //  T{ MAX-INT 1 0 WITHIN -> TRUE }T
  //  T{ MAX-INT 1 1 WITHIN -> FALSE }T
  //  T{ MAX-INT 1 MAX-INT WITHIN -> FALSE }T
  //  T{ MAX-INT MAX-INT MIN-INT WITHIN -> TRUE }T
  //  T{ MAX-INT MAX-INT 0 WITHIN -> TRUE }T
  //  T{ MAX-INT MAX-INT 1 WITHIN -> TRUE }T
  //  T{ MAX-INT MAX-INT MAX-INT WITHIN -> FALSE }T
}

TEST(unused) {
  //  VARIABLE UNUSED0
  //  T{ UNUSED DROP -> }T
  //  T{ ALIGN UNUSED UNUSED0 ! 0 , UNUSED CELL+ UNUSED0 @ = -> TRUE }T
  //  T{ UNUSED UNUSED0 ! 0 C, UNUSED CHAR+ UNUSED0 @ =
  //           -> TRUE }T  \ aligned -> unaligned
  //  T{ UNUSED UNUSED0 ! 0 C, UNUSED CHAR+ UNUSED0 @ = -> TRUE }T \ unaligned
  //  -> ?
}

TEST(again) {
  ": AG0 701 begin dup 7 mod 0= if exit then 1+ again ;"_s;
  TEST_ASSERT_EQUAL(0, shi::size());
  "AG0"_s;
  TEST_ASSERT_EQUAL(1, shi::size());
  TEST_ASSERT_EQUAL(707, shi::top());

  shi::clear();
}

TEST(marker) {
  //  T{ : MA? BL WORD FIND NIP 0<> ; -> }T
  //  T{ MARKER MA0 -> }T
  //  T{ : MA1 111 ; -> }T
  //  T{ MARKER MA2 -> }T
  //  T{ : MA1 222 ; -> }T
  //  T{ MA? MA0 MA? MA1 MA? MA2 -> TRUE TRUE TRUE }T
  //  T{ MA1 MA2 MA1 -> 222 111 }T
  //  T{ MA? MA0 MA? MA1 MA? MA2 -> TRUE TRUE FALSE }T
  //  T{ MA0 -> }T
  //  T{ MA? MA0 MA? MA1 MA? MA2 -> FALSE FALSE FALSE }T
}

TEST(q_do) {
  //  : QD ?DO I LOOP ;
  //  T{ 789 789 QD -> }T
  //  T{ -9876 -9876 QD -> }T
  //  T{ 5 0 QD -> 0 1 2 3 4 }T
  //
  //  : QD1 ?DO I 10 +LOOP ;
  //  T{ 50 1 QD1 -> 1 11 21 31 41 }T
  //  T{ 50 0 QD1 -> 0 10 20 30 40 }T
  //
  //  : QD2 ?DO I 3 > IF LEAVE ELSE I THEN LOOP ;
  //  T{ 5 -1 QD2 -> -1 0 1 2 3 }T
  //
  //  : QD3 ?DO I 1 +LOOP ;
  //  T{ 4  4 QD3 -> }T
  //  T{ 4  1 QD3 -> 1 2 3 }T
  //  T{ 2 -1 QD3 -> -1 0 1 }T
  //
  //  : QD4 ?DO I -1 +LOOP ;
  //  T{  4 4 QD4 -> }T
  //  T{  1 4 QD4 -> 4 3 2 1 }T
  //  T{ -1 2 QD4 -> 2 1 0 -1 }T
  //
  //  : QD5 ?DO I -10 +LOOP ;
  //  T{   1 50 QD5 -> 50 40 30 20 10 }T
  //  T{   0 50 QD5 -> 50 40 30 20 10 0 }T
  //  T{ -25 10 QD5 -> 10 0 -10 -20 }T
  //
  //  VARIABLE ITERS
  //  VARIABLE INCRMNT
  //
  //  : QD6 ( limit start increment -- )
  //     INCRMNT !
  //     0 ITERS !
  //     ?DO
  //        1 ITERS +!
  //        I
  //        ITERS @  6 = IF LEAVE THEN
  //        INCRMNT @
  //     +LOOP ITERS @
  //  ;
  //
  //  T{  4  4 -1 QD6 -> 0 }T
  //  T{  1  4 -1 QD6 -> 4 3 2 1 4 }T
  //  T{  4  1 -1 QD6 -> 1 0 -1 -2 -3 -4 6 }T
  //  T{  4  1  0 QD6 -> 1 1 1 1 1 1 6 }T
  //  T{  0  0  0 QD6 -> 0 }T
  //  T{  1  4  0 QD6 -> 4 4 4 4 4 4 6 }T
  //  T{  1  4  1 QD6 -> 4 5 6 7 8 9 6 }T
  //  T{  4  1  1 QD6 -> 1 2 3 3 }T
  //  T{  4  4  1 QD6 -> 0 }T
  //  T{  2 -1 -1 QD6 -> -1 -2 -3 -4 -5 -6 6 }T
  //  T{ -1  2 -1 QD6 -> 2 1 0 -1 4 }T
  //  T{  2 -1  0 QD6 -> -1 -1 -1 -1 -1 -1 6 }T
  //  T{ -1  2  0 QD6 -> 2 2 2 2 2 2 6 }T
  //  T{ -1  2  1 QD6 -> 2 3 4 5 6 7 6 }T
  //  T{ 2 -1 1 QD6 -> -1 0 1 3 }T
}

TEST(buffer) {
  //  T{ 8 BUFFER: BUF:TEST -> }T
  //  T{ BUF:TEST DUP ALIGNED = -> TRUE }T
  //  T{ 111 BUF:TEST ! 222 BUF:TEST CELL+ ! -> }T
  //  T{ BUF:TEST @ BUF:TEST CELL+ @ -> 111 222 }T
}

TEST(value__to) {
  //  T{ 111 VALUE VAL1 -999 VALUE VAL2 -> }T
  //  T{ VAL1 -> 111 }T
  //  T{ VAL2 -> -999 }T
  //  T{ 222 TO VAL1 -> }T
  //  T{ VAL1 -> 222 }T
  //  T{ : VD1 VAL1 ; -> }T
  //  T{ VD1 -> 222 }T
  //  T{ : VD2 TO VAL2 ; -> }T
  //  T{ VAL2 -> -999 }T
  //  T{ -333 VD2 -> }T
  //  T{ VAL2 -> -333 }T
  //  T{ VAL1 -> 222 }T
  //  T{ 123 VALUE VAL3 IMMEDIATE VAL3 -> 123 }T
  //  T{ : VD3 VAL3 LITERAL ; VD3 -> 123 }T
}

TEST(case__of__endof__endcase) {
  ": CS1 case 1 of 111 endof 2 of 222 endof 3 of 333 endof >r 999 r> endcase ;"_s;

  "1 CS1"_s;
  TEST_ASSERT_EQUAL(1, shi::size());
  TEST_ASSERT_EQUAL(111, shi::top());
  "2 CS1"_s;
  TEST_ASSERT_EQUAL(2, shi::size());
  TEST_ASSERT_EQUAL(222, shi::top());
  TEST_ASSERT_EQUAL(111, shi::top(1));
  "3 CS1"_s;
  TEST_ASSERT_EQUAL(3, shi::size());
  TEST_ASSERT_EQUAL(333, shi::top());
  TEST_ASSERT_EQUAL(222, shi::top(1));
  TEST_ASSERT_EQUAL(111, shi::top(2));
  "4 CS1"_s;
  TEST_ASSERT_EQUAL(4, shi::size());
  TEST_ASSERT_EQUAL(999, shi::top());
  TEST_ASSERT_EQUAL(333, shi::top(1));
  TEST_ASSERT_EQUAL(222, shi::top(2));
  TEST_ASSERT_EQUAL(111, shi::top(3));

  ": CS2 >r case -1 of case r@ 1 of 100 endof 2 of 200 endof >r -300 r> endcase endof -2 of case r@ 1 of -99 endof >r -199 r> endcase endof >r 299 r> endcase r> drop ;"_s;
  TEST_ASSERT_EQUAL(4, shi::size());

  "-1 1 CS2"_s;
  TEST_ASSERT_EQUAL(5, shi::size());
  TEST_ASSERT_EQUAL(100, shi::top());
  TEST_ASSERT_EQUAL(999, shi::top(1));
  TEST_ASSERT_EQUAL(333, shi::top(2));
  TEST_ASSERT_EQUAL(222, shi::top(3));
  TEST_ASSERT_EQUAL(111, shi::top(4));
  "-1 2 CS2"_s;
  TEST_ASSERT_EQUAL(6, shi::size());
  TEST_ASSERT_EQUAL(200, shi::top());
  TEST_ASSERT_EQUAL(100, shi::top(1));
  TEST_ASSERT_EQUAL(999, shi::top(2));
  TEST_ASSERT_EQUAL(333, shi::top(3));
  TEST_ASSERT_EQUAL(222, shi::top(4));
  TEST_ASSERT_EQUAL(111, shi::top(5));
  "-1 3 CS2"_s;
  TEST_ASSERT_EQUAL(7, shi::size());
  TEST_ASSERT_EQUAL(-300, shi::top());
  TEST_ASSERT_EQUAL(200, shi::top(1));
  TEST_ASSERT_EQUAL(100, shi::top(2));
  TEST_ASSERT_EQUAL(999, shi::top(3));
  TEST_ASSERT_EQUAL(333, shi::top(4));
  TEST_ASSERT_EQUAL(222, shi::top(5));
  TEST_ASSERT_EQUAL(111, shi::top(6));
  "-2 1 CS2"_s;
  TEST_ASSERT_EQUAL(8, shi::size());
  TEST_ASSERT_EQUAL(-99, shi::top());
  TEST_ASSERT_EQUAL(-300, shi::top(1));
  TEST_ASSERT_EQUAL(200, shi::top(2));
  TEST_ASSERT_EQUAL(100, shi::top(3));
  TEST_ASSERT_EQUAL(999, shi::top(4));
  TEST_ASSERT_EQUAL(333, shi::top(5));
  TEST_ASSERT_EQUAL(222, shi::top(6));
  TEST_ASSERT_EQUAL(111, shi::top(7));
  "-2 2 CS2"_s;
  TEST_ASSERT_EQUAL(9, shi::size());
  TEST_ASSERT_EQUAL(-199, shi::top());
  TEST_ASSERT_EQUAL(-99, shi::top(1));
  TEST_ASSERT_EQUAL(-300, shi::top(2));
  TEST_ASSERT_EQUAL(200, shi::top(3));
  TEST_ASSERT_EQUAL(100, shi::top(4));
  TEST_ASSERT_EQUAL(999, shi::top(5));
  TEST_ASSERT_EQUAL(333, shi::top(6));
  TEST_ASSERT_EQUAL(222, shi::top(7));
  TEST_ASSERT_EQUAL(111, shi::top(8));
  "0 2 CS2"_s;
  TEST_ASSERT_EQUAL(10, shi::size());
  TEST_ASSERT_EQUAL(299, shi::top());
  TEST_ASSERT_EQUAL(-199, shi::top(1));
  TEST_ASSERT_EQUAL(-99, shi::top(2));
  TEST_ASSERT_EQUAL(-300, shi::top(3));
  TEST_ASSERT_EQUAL(200, shi::top(4));
  TEST_ASSERT_EQUAL(100, shi::top(5));
  TEST_ASSERT_EQUAL(999, shi::top(6));
  TEST_ASSERT_EQUAL(333, shi::top(7));
  TEST_ASSERT_EQUAL(222, shi::top(8));
  TEST_ASSERT_EQUAL(111, shi::top(9));

  ": CS3 case 1- false of 11 endof 1- false of 22 endof 1- false of 33 endof 44 swap endcase ;"_s;

  "1 CS3"_s;
  TEST_ASSERT_EQUAL(11, shi::top());
  "2 CS3"_s;
  TEST_ASSERT_EQUAL(22, shi::top());
  "3 CS3"_s;
  TEST_ASSERT_EQUAL(33, shi::top());
  "9 CS3"_s;
  TEST_ASSERT_EQUAL(44, shi::top());

  shi::clear();

  ": CS4 case endcase ; 1 CS4"_s;
  TEST_ASSERT_EQUAL(0, shi::size());
  ": CS5 case 2 swap endcase ; 1 CS5"_s;
  TEST_ASSERT_EQUAL(2, shi::top());
  shi::clear();
  ": CS6 case 1 of endof 2 endcase ; 1 CS6"_s;
  TEST_ASSERT_EQUAL(0, shi::size());
  ": CS7 case 3 of endof 2 endcase ; 1 CS7"_s;
  TEST_ASSERT_EQUAL(1, shi::top());

  shi::clear();
}

TEST(colon_noname__recurse) {
  //  VARIABLE NN1
  //  VARIABLE NN2
  //  :NONAME 1234 ; NN1 !
  //  :NONAME 9876 ; NN2 !
  //  T{ NN1 @ EXECUTE -> 1234 }T
  //  T{ NN2 @ EXECUTE -> 9876 }T
  //
  //  T{ :NONAME ( n -- 0,1,..n ) DUP IF DUP >R 1- RECURSE R> THEN ;
  //     CONSTANT RN1 -> }T
  //  T{ 0 RN1 EXECUTE -> 0 }T
  //  T{ 4 RN1 EXECUTE -> 0 1 2 3 4 }T
  //
  //  :NONAME  ( n -- n1 )    \ Multiple RECURSEs in one definition
  //     1- DUP
  //     CASE 0 OF EXIT ENDOF
  //          1 OF 11 SWAP RECURSE ENDOF
  //          2 OF 22 SWAP RECURSE ENDOF
  //          3 OF 33 SWAP RECURSE ENDOF
  //          DROP ABS RECURSE EXIT
  //     ENDCASE
  //  ; CONSTANT RN2
  //
  //  T{  1 RN2 EXECUTE -> 0 }T
  //  T{  2 RN2 EXECUTE -> 11 0 }T
  //  T{  4 RN2 EXECUTE -> 33 22 11 0 }T
  //  T{ 25 RN2 EXECUTE -> 33 22 11 0 }T
}
