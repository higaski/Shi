#include <cassert>
#include <cstdint>
#include "stm32f4xx_hal.h"

extern "C" void shi_write_text(uint8_t* data_begin,
                               uint8_t* data_end,
                               uint8_t* text_begin) {
  asm volatile("nop");

  HAL_FLASH_Unlock();

  // Write words
  while (data_begin <= data_end - 4) {
    uint32_t adr = reinterpret_cast<uint32_t>(text_begin);
    text_begin += 4;

    uint32_t data =
        reinterpret_cast<uint32_t>(*reinterpret_cast<uint32_t*>(data_begin));
    data_begin += 4;

    HAL_StatusTypeDef status =
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, adr, data);
    asm volatile("nop");
  }

  // Write left halfword
  if (data_end - data_begin == 2) {
    uint32_t adr = reinterpret_cast<uint32_t>(text_begin);
    uint16_t data =
        reinterpret_cast<uint16_t>(*reinterpret_cast<uint16_t*>(data_begin));
    HAL_StatusTypeDef status =
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, adr, data);
    asm volatile("nop");
  } else if (data_end - data_begin)
    assert(false);

  HAL_FLASH_Lock();
}
