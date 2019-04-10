#include "quick_n_dirty.hpp"
#include <array>
#include "kB_literal.hpp"
#include "main.h"
#include "shi.hpp"

extern "C" void c_quick_n_dirty();
void cpp_quick_n_dirty();

namespace {

alignas(4) std::array<uint8_t, 32_kB> data{};

}  // namespace

void quick_n_dirty() {
  shi::init({.data_begin = (uint32_t)(begin(data)),
             .data_end = (uint32_t)(end(data)),
             .text_begin = FLASH_END - 32_kB,
             .text_end = FLASH_END});

  c_quick_n_dirty();
  shi::clear();
  cpp_quick_n_dirty();
}

