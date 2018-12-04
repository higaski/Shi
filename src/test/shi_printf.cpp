#include <cstdio>

extern "C" int shi_printf(char const* format) {
  return printf(format);
}
