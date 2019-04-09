#include <cstdio>
#include <cstdlib>

int c_bench();
int lua_bench();
int mp_bench();
int shi_bench();

int bench() {
  c_bench();
  lua_bench();
  mp_bench();
  shi_bench();

  return 0;
}

extern "C" [[noreturn]] void bench_assert_func(char const* file,
                                               int line,
                                               char const* func,
                                               char const* failedexpr) {
  printf("assertion \"%s\" failed: file \"%s\", line %d%s%s\n",
         failedexpr,
         file,
         line,
         func ? ", function: " : "",
         func ? func : "");
  abort();
}
