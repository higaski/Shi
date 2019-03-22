#include "mpapi.h"

extern "C" void mp_count_primes_by_trial_division();
extern "C" void mp_lerp();

void mp_bench() {
  mp_init();

  mp_count_primes_by_trial_division();
  mp_lerp();

  mp_deinit();
}
