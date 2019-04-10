int c_count_primes_by_trial_division();
int c_lerp();

int c_bench() {
  int retval{};

  asm volatile("nop");
  retval |= c_count_primes_by_trial_division();  // 131.8ms
  asm volatile("nop");
  retval |= c_lerp();  // 615us
  asm volatile("nop");

  return retval;
}
