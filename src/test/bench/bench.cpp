#include <cstdio>
#include <cstdlib>

int c_bench();
int lua_bench();
int mp_bench();
int shi_bench();

int bench() {
  int retval{c_bench() | lua_bench() | mp_bench() | shi_bench()};

  if (retval)
    printf("some error during benching...\n");

  return retval;
}
