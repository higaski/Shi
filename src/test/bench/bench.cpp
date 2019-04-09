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
