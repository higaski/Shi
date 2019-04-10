#include "bench.h"
#include "count_primes_by_trial_division_data.h"
#include "mpapi.h"

#define xstr(s) str(s)
#define str(s) #s

static int retval = 0;

STATIC mp_obj_t primes_result(mp_obj_t n) {
  retval = mp_obj_get_int(n);
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(primes_obj, primes_result);

STATIC const mp_map_elem_t primes_globals_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_primes_module)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_primes_result), (mp_obj_t)&primes_obj},
};

STATIC MP_DEFINE_CONST_DICT(mp_module_primes_globals, primes_globals_table);

const mp_obj_module_t mp_primes_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t*)&mp_module_primes_globals,
};

int mp_count_primes_by_trial_division() {
  mp_dostring("import primes_module\n"
              "def is_prime(n):\n"
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

  mp_dostring("primes_module.primes_result(n)\n", MP_PARSE_FILE_INPUT);

  return retval == RESULT_PRIMES_TILL ? 0 : 1;
}
