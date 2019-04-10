#include <cstdint>
#include <cstdio>
#include "bench.h"

namespace {

[[gnu::noinline]] bool is_prime(uint32_t n) {
  if (!(n & 1) || n < 2)
    return n == 2;

  // comparing p*p <= n can overflow
  for (uint32_t p = 3; p <= n / p; p += 2)
    if (!(n % p))
      return false;
  return true;
}

[[gnu::noinline]] uint32_t count_primes(uint32_t n) {
  uint32_t retval{0};

  for (uint32_t i = 0; i <= n; ++i)
    if (is_prime(i))
      ++retval;

  return retval;
}

}  // namespace

int c_count_primes_by_trial_division() {
  volatile auto n{100000};
  START_CYC_CNT();
  auto retval{count_primes(n)};
  STOP_CYC_CNT();

  return retval == 9592 ? 0 : 1;
}
