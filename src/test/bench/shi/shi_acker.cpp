#include <cstdio>
#include <cstdlib>
#include "bench.h"
#include "shi.hpp"

using namespace shi::literals;

int shi_acker() {
  ": ack "
  "over 0= if nip 1+ exit then "
  "swap 1- swap "
  "dup 0= if 1+ recurse exit then "
  "1- over 1+ swap recurse recurse ;"_s;

  // That one's faster, but I consider it "cheating"
  //  ": ack1 "
  //  "over "
  //  "0 over = if drop nip 1+     else "
  //  "1 over = if drop nip 2 +    else "
  //  "2 over = if drop nip 2* 3 + else "
  //  "3 over = if drop swap 5 + swap lshift 3 - else "
  //  "  drop swap 1- swap dup "
  //  "  if "
  //  "    1- over 1+ swap recurse recurse exit "
  //  "  else "
  //  "    1+ recurse exit "
  //  "  then "
  //  "then then then then "
  //  ";"_s;

  START_CYC_CNT();
  int retval{"ack"_w(3, 2)};
  STOP_CYC_CNT();

  return retval == 29 ? 0 : 1;
}
