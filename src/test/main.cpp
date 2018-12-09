#include <cstdio>
#include "forth2012_test_suite.hpp"
#include "shi.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

#define SHI_FLASH_START (0x08000000 + 0x00040000 - 0x8000)
#define SHI_FLASH_END (0x08000000 + 0x00040000)
#define SHI_RAM_START (0x20004000)
#define SHI_RAM_END (0x20004000 + 0x00004000)

using shi::operator""_fs;

void shi_test();
void shi_test_compile_to_flash();
void SystemClock_Config();
void _Error_Handler(char* file, int line);

TEST(unused0) {
  ",toram"_fs;
  "unused"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::size());
  auto top = shi::top();
  TEST_ASSERT_EQUAL_INT(SHI_RAM_END - SHI_RAM_START, shi::top());

  ",toflash"_fs;
  "unused"_fs;
  TEST_ASSERT_EQUAL_INT(2, shi::size());
  top = shi::top();
  TEST_ASSERT_EQUAL_INT(SHI_FLASH_END - SHI_FLASH_START, shi::top());

  ",toram"_fs;

  shi::clear();
}

TEST(unused1) {
  ",toram"_fs;
  "unused"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::size());
  TEST_ASSERT_LESS_THAN_INT(SHI_RAM_END - SHI_RAM_START, shi::top());

  shi::clear();
}

TEST(numeric_notation) {
  "#1289"_fs;
  TEST_ASSERT_EQUAL_INT(1289, shi::top());

  "#12346789"_fs;
  TEST_ASSERT_EQUAL_INT(12346789, shi::top());

  "#-1289"_fs;
  TEST_ASSERT_EQUAL_INT(-1289, shi::top());

  "$12EF"_fs;  // Upper case hex only!
  TEST_ASSERT_EQUAL_INT(0x12eF, shi::top());

  "$-12EF"_fs;
  TEST_ASSERT_EQUAL_INT(-0x12eF, shi::top());

  "%10010110"_fs;
  TEST_ASSERT_EQUAL_INT(0b10010110, shi::top());

  "%-10010110"_fs;
  TEST_ASSERT_EQUAL_INT(-0b10010110, shi::top());

  shi::clear();
}

TEST(top) {
  "13"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::size());
  TEST_ASSERT_EQUAL_INT(13, shi::top());

  "170"_fs;
  TEST_ASSERT_EQUAL_INT(2, shi::size());
  TEST_ASSERT_EQUAL_INT(170, shi::top());
  TEST_ASSERT_EQUAL_INT(13, shi::top(1));
  TEST_ASSERT_EQUAL_INT(13, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(2, shi::size());

  "38"_fs;
  TEST_ASSERT_EQUAL_INT(3, shi::size());
  TEST_ASSERT_EQUAL_INT(38, shi::top());
  TEST_ASSERT_EQUAL_INT(170, shi::top(1));
  TEST_ASSERT_EQUAL_INT(170, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(13, shi::top(2));
  TEST_ASSERT_EQUAL_INT(13, shi::top(-2));

  shi::clear();
}

TEST(if_else_then) {
  ": gi2 if 123 else 234 then ;"_fs;
  TEST_ASSERT_EQUAL_INT(0, shi::size());
  TEST_ASSERT_TRUE(shi::empty());

  "-1 gi2"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::size());
  TEST_ASSERT_EQUAL_INT(123, shi::top());

  "1 gi2"_fs;
  TEST_ASSERT_EQUAL_INT(2, shi::size());
  TEST_ASSERT_EQUAL_INT(123, shi::top());
  TEST_ASSERT_EQUAL_INT(123, shi::top(-1));

  "0 gi2"_fs;
  TEST_ASSERT_EQUAL_INT(3, shi::size());
  TEST_ASSERT_EQUAL_INT(234, shi::top());
  TEST_ASSERT_EQUAL_INT(123, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(123, shi::top(-2));

  ": melse if 1 else 2 else 3 else 4 else 5 then ;"_fs;

  "false melse"_fs;
  TEST_ASSERT_EQUAL_INT(5, shi::size());
  TEST_ASSERT_EQUAL_INT(4, shi::top());
  TEST_ASSERT_EQUAL_INT(2, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(234, shi::top(-2));
  TEST_ASSERT_EQUAL_INT(123, shi::top(-3));
  TEST_ASSERT_EQUAL_INT(123, shi::top(-4));

  "true melse"_fs;
  TEST_ASSERT_EQUAL_INT(8, shi::size());
  TEST_ASSERT_EQUAL_INT(5, shi::top());
  TEST_ASSERT_EQUAL_INT(3, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(1, shi::top(-2));
  TEST_ASSERT_EQUAL_INT(4, shi::top(-3));
  TEST_ASSERT_EQUAL_INT(2, shi::top(-4));
  TEST_ASSERT_EQUAL_INT(234, shi::top(-5));
  TEST_ASSERT_EQUAL_INT(123, shi::top(-6));
  TEST_ASSERT_EQUAL_INT(123, shi::top(-7));

  shi::clear();
}

TEST(loop) {
  ": gd1 do i loop ;"_fs;
  TEST_ASSERT_TRUE(shi::empty());

  "4 1 gd1"_fs;
  TEST_ASSERT_EQUAL_INT(3, shi::size());
  TEST_ASSERT_EQUAL_INT(3, shi::top());
  TEST_ASSERT_EQUAL_INT(2, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(1, shi::top(-2));

  "2 -1 gd1"_fs;
  TEST_ASSERT_EQUAL_INT(6, shi::size());
  TEST_ASSERT_EQUAL_INT(1, shi::top(0));
  TEST_ASSERT_EQUAL_INT(0, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(-1, shi::top(-2));
  TEST_ASSERT_EQUAL_INT(3, shi::top(-3));
  TEST_ASSERT_EQUAL_INT(2, shi::top(-4));
  TEST_ASSERT_EQUAL_INT(1, shi::top(-5));

  shi::clear();
}

TEST(leave) {
  ": gd5 123 swap 0 do i 4 > if drop 234 leave then loop ;"_fs;
  TEST_ASSERT_TRUE(shi::empty());

  "1 gd5"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::size());
  TEST_ASSERT_EQUAL_INT(123, shi::top());

  "5 gd5"_fs;
  TEST_ASSERT_EQUAL_INT(2, shi::size());
  TEST_ASSERT_EQUAL_INT(123, shi::top());
  TEST_ASSERT_EQUAL_INT(123, shi::top(-1));

  "6 gd5"_fs;
  TEST_ASSERT_EQUAL_INT(3, shi::size());
  TEST_ASSERT_EQUAL_INT(234, shi::top());
  TEST_ASSERT_EQUAL_INT(123, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(123, shi::top(-2));

  shi::clear();
}

TEST(variable) {
  "variable v1"_fs;
  TEST_ASSERT_TRUE(shi::empty());

  "123 v1 !"_fs;
  TEST_ASSERT_TRUE(shi::empty());

  "v1 @"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::size());
  TEST_ASSERT_EQUAL_INT(123, shi::top());

  shi::clear();
}

TEST(constant) {
  "123 constant x123"_fs;
  TEST_ASSERT_TRUE(shi::empty());

  "x123"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::size());
  TEST_ASSERT_EQUAL_INT(123, shi::top());

  ": equ constant ;"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::size());

  "x123 equ y123"_fs;
  TEST_ASSERT_EQUAL_INT(1, shi::size());

  "y123"_fs;
  TEST_ASSERT_EQUAL_INT(2, shi::size());
  TEST_ASSERT_EQUAL_INT(123, shi::top());
  TEST_ASSERT_EQUAL_INT(123, shi::top(-1));

  shi::clear();
}

// This looks super promising for writing flash memory stuff @ QEMU...
// https://dangokyo.me/2018/03/27/qemu-internal-memory-region-address-space-and-qemu-io/#more-1697

int main() {
  // Reset of all peripherals, Initializes the Flash interface and the Systick
  HAL_Init();
  // Configure the system clock
  SystemClock_Config();

  asm volatile("nop");
  shi::init({.ram_begin = SHI_RAM_START,
             .ram_end = SHI_RAM_END,
             .flash_begin = SHI_FLASH_START,
             .flash_end = SHI_FLASH_END});

//  "17 create seventeen ,"_fs;
//  "seventeen"_fs;
//
//  ": my_constant create , does> @ ;"_fs;
//  "7 my_constant CON"_fs;
//  "CON"_fs;

  //  semihosting_io();

  //  RUN_TEST(unused0);  // Needs to run at the very top

  //  RUN_TEST(numeric_notation);
  //  RUN_TEST(top);
  //  RUN_TEST(if_else_then);
  //  RUN_TEST(loop);
  //  RUN_TEST(leave);
  //  RUN_TEST(case_);
  //  RUN_TEST(variable);
  //  RUN_TEST(constant);

  //  RUN_TEST(unused1);  // Can run anywhere but at the top

  return forth2012_test_suite();
}

void shi_test_compile_to_flash() {
  asm volatile("nop");

  // Compile to FLASH
  //  shi::evaluate(",toflash");
  //  shi::evaluate(": p13 13 ;");
  //  shi::evaluate(": p1314 p13 14 ;");
  //  shi::stack_print(printf);  // Empty
  //  shi::evaluate("p1314");    // 13 14
  //  shi::stack_print(printf);
  //  shi::evaluate(",toram");

  asm volatile("nop");
}

void shi_test() {
  // Tick (returns xt of definition)
  shi::evaluate("' cs1");  // some ... ram address
  shi::stack_print(printf);
  shi::evaluate("' gagsi_undefined");  // should get error message
  shi::stack_print(printf);
  shi::clear();
  asm volatile("nop");

  // Checking if a definition exists is rather complicated if you don't want to
  // execute it... You've got to push source on the stack, parse the following
  // name of the definition an then execute find on it...
  //
  // This returns either
  // ( -- token-addr false ) if nothing was found or
  // ( -- xt flags )         if the definition was found
  asm volatile("nop");
  shi::evaluate("source parse cs1 find");
  shi::stack_print(printf);

  // In theory we can now create a new definition, based upon if cs1 exists or
  // not
  auto f_exists = shi::top();
  if (f_exists)
    printf("definition cs1 found\n");
  else
    printf("definition cs1 not found\n");
  shi::evaluate("2drop");
  shi::stack_print(printf);

  // Lets try that again with a function we haven't defined yet
  shi::evaluate("source parse foo find");
  shi::stack_print(printf);
  f_exists = shi::top();
  if (f_exists)
    printf("definition foo found\n");
  else
    printf("definition foo not found\n");
  shi::clear();
  asm volatile("nop");

  // C variable
  asm volatile("nop");
  volatile uint32_t stars = 42;
  // equal to push(&stars) and evaluate("c-variable stars");
  shi::cvariable("stars", &stars);
  shi::stack_print(printf);
  shi::evaluate("stars @");
  shi::stack_print(printf);
  shi::evaluate("1 + stars !");
  shi::stack_print(printf);
  printf("Forth look how stars (our c-variable) turned out: %d\n", stars);
  shi::clear();

  // Stack functions
  asm volatile("nop");
  volatile int32_t int32{};
  volatile float f32_1{};
  volatile float f32_2{};
  shi::push(10);  // Push integer
  shi::stack_print(printf);
  int32 = shi::top();  // Read integer
  shi::push(32.4f);    // Push float
  shi::stack_print(printf);
  //  f32_1 = shi::top<float>();  // Read float
  //  shi::top(f32_2);            // Read float another (safer) way
  shi::clear();

  asm volatile("nop");
}

/// Overwritten SysTick handler
extern "C" void __attribute__((section(".after_vectors"))) SysTick_Handler() {
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
}

/// This function is executed in case of error occurrence
/// \param  file  The file name as string
/// \param  line  The line in file as a number
void _Error_Handler(char* file, int line) {
  while (1) {
  }
}

/// System Clock Configuration
void SystemClock_Config() {
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  // Configure the main internal regulator output voltage
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  // Initializes the CPU, AHB and APB busses clocks
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    _Error_Handler(__FILE__, __LINE__);
  }

  // Initializes the CPU, AHB and APB busses clocks
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
    _Error_Handler(__FILE__, __LINE__);
  }

  // Configure the Systick interrupt time
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

  // Configure the Systick
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  // SysTick_IRQn interrupt configuration
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}
