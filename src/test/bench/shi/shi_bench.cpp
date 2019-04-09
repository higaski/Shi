#include "main.h"
#include "shi.hpp"

void shi_count_primes_by_trial_division();
void shi_lerp();

namespace {

alignas(4) std::array<uint8_t, 32 * 1024> data{};

}  // namespace

int shi_bench() {
  shi::init({.data_begin = reinterpret_cast<uint32_t>(begin(data)),
             .data_end = reinterpret_cast<uint32_t>(end(data)),
             .text_begin = FLASH_END - (32 * 1024),
             .text_end = FLASH_END});

  shi_count_primes_by_trial_division();
  shi_lerp();

  return 0;
}
