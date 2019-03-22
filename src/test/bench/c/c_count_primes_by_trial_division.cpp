#include <cstdint>
#include <cstdio>
#include "bench.h"
#include "count_primes_by_trial_division_data.h"

bool is_prime(uint32_t n) {
  if (!(n & 1) || n < 2)
    return n == 2;

  // comparing p*p <= n can overflow
  for (uint32_t p = 3; p <= n / p; p += 2)
    if (!(n % p))
      return false;
  return true;
}

uint32_t count_primes(uint32_t n) {
  uint32_t retval{0};

  for (uint32_t i = 0; i <= n; ++i)
    if (is_prime(i))
      ++retval;

  return retval;
}

void c_count_primes_by_trial_division() {
  START_CYC_CNT();
  uint32_t retval{count_primes(COUNT_PRIMES_TILL)};
  STOP_CYC_CNT();

  bench_assert(retval == RESULT_PRIMES_TILL);
}
