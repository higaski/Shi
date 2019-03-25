#include <cstdio>
#include <cstdlib>
#include "bench.h"
#include "shi.hpp"

using shi::operator""_s;
using shi::operator""_w;

// A(0, 0) = 1
// A(0, 1) = 2
// A(0, 2) = 3
// A(0, 3) = 4
// A(0, 4) = 5
// A(0, 5) = 6
// A(1, 0) = 2
// A(1, 1) = 3
// A(1, 2) = 4
// A(1, 3) = 5
// A(1, 4) = 6
// A(2, 0) = 3
// A(2, 1) = 5
// A(2, 2) = 7
// A(2, 3) = 9
// A(3, 0) = 5
// A(3, 1) = 13
// A(3, 2) = 29
// A(4, 0) = 13
// A(4, 1) = 65533

void shi_acker() {
  // die variante geht nicht???
  // und is irgendwie auch davon abhängig wie viele werte am stack liegen?
  // hä
  ": acker "
  "over 0= if nip 1+ exit then "
  "swap 1- swap "
  "dup 0= if 1+ recurse exit then "
  "1- over 1+ swap recurse recurse ;"_s;

  int32_t r;
  asm volatile("nop");
  r = "acker"_w(0, 0);  // 1
  printf("%d\n", r);
  r = "acker"_w(0, 1);  // 2
  printf("%d\n", r);
  r = "acker"_w(0, 2);  // 3
  printf("%d\n", r);
  r = "acker"_w(0, 3);  // 4
  printf("%d\n", r);
  r = "acker"_w(0, 4);  // 5
  printf("%d\n", r);
  r = "acker"_w(0, 5);  // 6
  printf("%d\n", r);
  r = "acker"_w(1, 0);  // 2
  printf("%d\n", r);
  r = "acker"_w(1, 1);  // 3
  printf("%d\n", r);
  r = "acker"_w(1, 2);  // 4
  printf("%d\n", r);
  r = "acker"_w(1, 3);  // 5
  printf("%d\n", r);
  r = "acker"_w(1, 4);  // 6
  printf("%d\n", r);
  r = "acker"_w(2, 0);  // 3
  printf("%d\n", r);
  r = "acker"_w(2, 1);  // 5
  printf("%d\n", r);
  r = "acker"_w(2, 2);  // 7
  printf("%d\n", r);
  r = "acker"_w(2, 3);  // 9
  printf("%d\n", r);
  r = "acker"_w(3, 0);  // 5
  printf("%d\n", r);
  r = "acker"_w(3, 1);  // 13
  printf("%d\n", r);
  r = "acker"_w(3, 2);  // 29
  printf("%d\n", r);
  r = "acker"_w(4, 0);  // 13
  printf("%d\n", r);

  // die aber scho? hä?
  ": ackermann "
  "over "
  "0 over = if drop nip 1+     else "
  "1 over = if drop nip 2 +    else "
  "2 over = if drop nip 2* 3 + else "
  "3 over = if drop swap 5 + swap lshift 3 - else "
  "  drop swap 1- swap dup "
  "  if "
  "    1- over 1+ swap recurse recurse exit "
  "  else "
  "    1+ recurse exit "
  "  then "
  "then then then then "
  ";"_s;

  asm volatile("nop");
  "2 3"_s;
  asm volatile("nop");
  "ackermann"_s;

  asm volatile("nop");
  "3 4"_s;
  asm volatile("nop");
  "ackermann"_s;
}
