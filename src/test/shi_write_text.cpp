#include <cassert>
#include <cstdint>
#include "stm32f4xx_hal.h"

/// Align value to alignment
///
/// \param  alignment Alignment
/// \param  value     Value to align
/// \return Aligned value
size_t align2(size_t alignment, size_t value) {
  return value + (alignment - value % alignment) % alignment;
}

extern "C" uint32_t shi_write_text(uint8_t* data_begin,
                                   uint8_t* data_end,
                                   uint8_t* text_begin) {
  asm volatile("nop");

  // size of definition to write
  size_t def_size = data_end - data_begin;
  asm volatile("nop");

  // link equals text_begin + def_size and maybe some alignment?
  // also same as retval?
  uint32_t link = align2(4, reinterpret_cast<size_t>(text_begin + def_size + 4));

  HAL_FLASH_Unlock();

  // Write link to flash (assuming 4byte alignment here...)
  uint32_t adr = reinterpret_cast<uint32_t>(text_begin);
  text_begin += 4;
  HAL_StatusTypeDef status =
      HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, adr, link);

  while (data_begin <= data_end - 4) {
    adr = reinterpret_cast<uint32_t>(text_begin);
    text_begin += 4;
    uint32_t data =
        reinterpret_cast<uint32_t>(*reinterpret_cast<uint32_t*>(data_begin));
    data_begin += 4;
    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, adr, data);
    asm volatile("nop");
  }

  if (data_end - data_begin == 2) {
    adr = reinterpret_cast<uint32_t>(text_begin);
    uint16_t data =
        reinterpret_cast<uint16_t>(*reinterpret_cast<uint16_t*>(data_begin));
    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, adr, data);
  } else if (data_end - data_begin)
    assert(false);

  HAL_FLASH_Lock();

  return link;
}
