#include "bench.h"
#include "count_primes_by_trial_division_data.h"
#include "mpapi.h"

#define xstr(s) str(s)
#define str(s) #s

int mp_count_primes_by_trial_division() {
  mp_dostring("def is_prime(n):\n"
              "    if (not (n & 1)) or (n < 2):\n"
              "        return n == 2\n"
              "    p = 3\n"
              "    while p <= n//p:\n"
              "        if not (n % p):\n"
              "            return False\n"
              "        p = p + 2\n"
              "    return True\n",
              MP_PARSE_FILE_INPUT);

  mp_dostring("def count_primes(n):\n"
              "    retval = 0\n"
              "    for i in range(0, n + 1):\n"
              "        if is_prime(i):\n"
              "            retval = retval + 1\n"
              "    return retval\n",
              MP_PARSE_FILE_INPUT);

  // clang-format off
  mp_obj_t count_primes_by_trial_division_bytecode = mp_compilestring(
      "n = count_primes("xstr(COUNT_PRIMES_TILL)")\n", MP_PARSE_FILE_INPUT);
  // clang-format on

  START_CYC_CNT();
  mp_dobytecode(count_primes_by_trial_division_bytecode);
  STOP_CYC_CNT();

  return 0;
}
