#pragma once

// Enable CYCCNT register
#define DWT_CYCCNT_EN() DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk

// Disable CYCCNT register
#define DWT_CYCCNT_DIS() DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk

// Get value from CYCCNT register
#define DWT_CYCCNT_GET() DWT->CYCCNT
