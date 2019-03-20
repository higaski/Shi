#include <cstdio>

extern "C" int shi_printf(char const* error) {
  return printf("'shi' %s", error);
}
