#pragma once

#include <cstddef>

constexpr size_t align(size_t alignment, size_t value) {
  return value + (alignment - value % alignment) % alignment;
}

constexpr size_t operator"" _kB(unsigned long long b) {
  return b * 1024;
}
