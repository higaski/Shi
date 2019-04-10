#pragma once

#include <cstddef>

constexpr size_t operator"" _kB(unsigned long long b) {
  return b * 1024;
}
