#include "bench.h"
#include "shi.hpp"

using namespace shi::literals;

int shi_count_primes_by_trial_division() {
  ": prime? dup dup "
  "1 and 0= swap 2 < or if 2 = "
  "else 3 "
  "begin 2dup / over swap > invert "
  "while 2dup mod 0= "
  "  if 2drop false exit "
  "  then 2 + "
  "repeat "
  "2drop "
  "true "
  "then "
  ";"_s;

  ": count_primes 0 swap 1+ 0 do i prime? if 1+ then loop ;"_s;

  START_CYC_CNT();
  int32_t retval{"count_primes"_w(100000)};
  STOP_CYC_CNT();

  return retval == 9592 ? 0 : 1;
}
