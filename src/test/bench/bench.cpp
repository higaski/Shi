#include <cstdio>
#include <cstdlib>

void c_bench();
void lua_bench();
void mp_bench();
void shi_bench();

void bench() {
  c_bench();
  lua_bench();
  mp_bench();
  //shi_bench();
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
