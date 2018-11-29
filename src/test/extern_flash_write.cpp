#include <cstdint>
#include "stm32f4xx_hal.h"

/// Align value to alignment
///
/// \param  alignment Alignment
/// \param  value     Value to align
/// \return Aligned value
size_t align(size_t const alignment, size_t const value) {
  return value + (alignment - value % alignment) % alignment;
}

extern "C" void extern_flash_write(uint32_t def_begin,
                                   uint32_t def_end,
                                   uint32_t flash_begin) {
  // size of definition to write
  size_t def_size = def_end - def_begin;
  asm volatile("nop");

  // before calling this function def_begin is incremented by 4
  // so the 0xFFFFFFFF link is gone (dont need it anyway)
  printf("%d\n", *reinterpret_cast<uint32_t*>(def_begin));

  // write link which equals flash_begin + def_size and maybe some alignment?
  // also same as retval?
  uint32_t next_link = align(4, flash_begin + def_size);

  HAL_FLASH_Unlock();



  HAL_FLASH_Lock();

  asm volatile("nop");
}
