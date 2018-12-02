#include <cstdio>

extern "C" int shi_printf(const char* format) {
  return printf(format);
}
