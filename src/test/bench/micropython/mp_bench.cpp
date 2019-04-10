#include "mpapi.h"

extern "C" int mp_count_primes_by_trial_division();
extern "C" int mp_lerp();

int mp_bench() {
  mp_init();

  int retval{};

  asm volatile("nop");
  retval |= mp_count_primes_by_trial_division();
  asm volatile("nop");
  retval |= mp_lerp();
  asm volatile("nop");

  mp_deinit();

  return retval;
}
