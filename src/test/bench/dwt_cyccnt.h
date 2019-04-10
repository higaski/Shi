#pragma once

#include <stdint.h>
#include <stdio.h>
#include "main.h"

// Enable CYCCNT register
#define DWT_CYCCNT_EN() DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk

// Disable CYCCNT register
#define DWT_CYCCNT_DIS() DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk

// Get value from CYCCNT register
#define DWT_CYCCNT_GET() DWT->CYCCNT

// Reset CYCCNT register
#define DWT_CYCCNT_RES() DWT->CYCCNT = 0

// Start cycle counter
#define START_CYC_CNT()                                                        \
  __disable_irq();                                                             \
  uint32_t it1, it2;                                                           \
  DWT_CYCCNT_RES();                                                            \
  DWT_CYCCNT_EN();                                                             \
  it1 = DWT_CYCCNT_GET();                                                      \
  HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_SET);

// Stop cycle counter
#define STOP_CYC_CNT()                                                         \
  HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_RESET);         \
  it2 = DWT_CYCCNT_GET() - it1;                                                \
  DWT_CYCCNT_DIS();                                                            \
  __enable_irq();                                                              \
  printf("%s took %u cyc\n", __PRETTY_FUNCTION__, it2);\
