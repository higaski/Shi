#include "kB_literal.hpp"
#include "main.h"
#include "shi.hpp"

int shi_acker();
int shi_count_primes_by_trial_division();
int shi_lerp();

namespace {

alignas(4) std::array<uint8_t, 32_kB> data{};

}  // namespace

int shi_bench() {
  shi::init({.data_begin = reinterpret_cast<uint32_t>(begin(data)),
             .data_end = reinterpret_cast<uint32_t>(end(data))});

  int retval{};

  asm volatile("nop");
  retval |= shi_acker();
  asm volatile("nop");
  retval |= shi_count_primes_by_trial_division();
  asm volatile("nop");
  retval |= shi_lerp();
  asm volatile("nop");

  return retval;
}
