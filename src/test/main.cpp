#include <cstdio>
#include "shi.hpp"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "unity.h"

void shi_test();
void shi_test_compile_to_flash();
void SystemClock_Config();
void _Error_Handler(char* file, int line);

#define TEST(name) void name()

TEST(number) {
  shi::evaluate("13");
  TEST_ASSERT_EQUAL_INT(1, shi::size());
  TEST_ASSERT_EQUAL_INT(13, shi::top());

  shi::clear();
}

TEST(number_hex) {
  shi::evaluate("$21");
  TEST_ASSERT_EQUAL_INT(0x21, shi::top());
  TEST_ASSERT_EQUAL_INT(1, shi::size());

  shi::clear();
}

TEST(number_bin) {
  shi::evaluate("%00011101");
  TEST_ASSERT_EQUAL_INT(1, shi::size());
  TEST_ASSERT_EQUAL_INT(0b0001'1101, shi::top());

  shi::clear();
}

TEST(top) {
  shi::evaluate("13");
  TEST_ASSERT_EQUAL_INT(1, shi::size());
  TEST_ASSERT_EQUAL_INT(13, shi::top());

  shi::evaluate("170");
  TEST_ASSERT_EQUAL_INT(2, shi::size());
  TEST_ASSERT_EQUAL_INT(170, shi::top());
  TEST_ASSERT_EQUAL_INT(13, shi::top(1));
  TEST_ASSERT_EQUAL_INT(13, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(2, shi::size());

  shi::evaluate("38");
  TEST_ASSERT_EQUAL_INT(3, shi::size());
  TEST_ASSERT_EQUAL_INT(38, shi::top());
  TEST_ASSERT_EQUAL_INT(170, shi::top(1));
  TEST_ASSERT_EQUAL_INT(170, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(13, shi::top(2));
  TEST_ASSERT_EQUAL_INT(13, shi::top(-2));

  shi::clear();
}

TEST(if_else_then) {
  shi::evaluate(": gi2 if 123 else 234 then ;");
  TEST_ASSERT_EQUAL_INT(0, shi::size());
  TEST_ASSERT_TRUE(shi::empty());

  shi::evaluate("-1 gi2");
  TEST_ASSERT_EQUAL_INT(1, shi::size());
  TEST_ASSERT_EQUAL_INT(123, shi::top());

  shi::evaluate("1 gi2");
  TEST_ASSERT_EQUAL_INT(2, shi::size());
  TEST_ASSERT_EQUAL_INT(123, shi::top());
  TEST_ASSERT_EQUAL_INT(123, shi::top(-1));

  shi::evaluate("0 gi2");
  TEST_ASSERT_EQUAL_INT(3, shi::size());
  TEST_ASSERT_EQUAL_INT(234, shi::top());
  TEST_ASSERT_EQUAL_INT(123, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(123, shi::top(-2));

  shi::evaluate(": melse if 1 else 2 else 3 else 4 else 5 then ;");

  shi::evaluate("false melse");
  TEST_ASSERT_EQUAL_INT(5, shi::size());
  TEST_ASSERT_EQUAL_INT(4, shi::top());
  TEST_ASSERT_EQUAL_INT(2, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(234, shi::top(-2));
  TEST_ASSERT_EQUAL_INT(123, shi::top(-3));
  TEST_ASSERT_EQUAL_INT(123, shi::top(-4));

  shi::evaluate("true melse");
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
  shi::evaluate(": gd1 do i loop ;");
  TEST_ASSERT_TRUE(shi::empty());

  shi::evaluate("4 1 gd1");
  TEST_ASSERT_EQUAL_INT(3, shi::size());
  TEST_ASSERT_EQUAL_INT(3, shi::top());
  TEST_ASSERT_EQUAL_INT(2, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(1, shi::top(-2));

  shi::evaluate("2 -1 gd1");
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
  shi::evaluate(": gd5 123 swap 0 do i 4 > if drop 234 leave then loop ;");
  TEST_ASSERT_TRUE(shi::empty());

  shi::evaluate("1 gd5");
  TEST_ASSERT_EQUAL_INT(1, shi::size());
  TEST_ASSERT_EQUAL_INT(123, shi::top());

  shi::evaluate("5 gd5");
  TEST_ASSERT_EQUAL_INT(2, shi::size());
  TEST_ASSERT_EQUAL_INT(123, shi::top());
  TEST_ASSERT_EQUAL_INT(123, shi::top(-1));

  shi::evaluate("6 gd5");
  TEST_ASSERT_EQUAL_INT(3, shi::size());
  TEST_ASSERT_EQUAL_INT(234, shi::top());
  TEST_ASSERT_EQUAL_INT(123, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(123, shi::top(-2));

  shi::clear();
}

TEST(case_) {
  shi::evaluate(": cs1 case 1 of 111 endof 2 of 222 endof 3 of 333 endof >r "
                "999 r> endcase ;");
  TEST_ASSERT_TRUE(shi::empty());

  shi::evaluate("1 cs1");  // 111
  TEST_ASSERT_EQUAL_INT(1, shi::size());
  TEST_ASSERT_EQUAL_INT(111, shi::top());

  shi::evaluate("2 cs1");  // 222
  TEST_ASSERT_EQUAL_INT(2, shi::size());
  TEST_ASSERT_EQUAL_INT(222, shi::top());
  TEST_ASSERT_EQUAL_INT(111, shi::top(-1));

  shi::evaluate("3 cs1");  // 333
  TEST_ASSERT_EQUAL_INT(3, shi::size());
  TEST_ASSERT_EQUAL_INT(333, shi::top());
  TEST_ASSERT_EQUAL_INT(222, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(111, shi::top(-2));

  shi::evaluate("4 cs1");  // 999
  TEST_ASSERT_EQUAL_INT(4, shi::size());
  TEST_ASSERT_EQUAL_INT(999, shi::top());
  TEST_ASSERT_EQUAL_INT(333, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(222, shi::top(-2));
  TEST_ASSERT_EQUAL_INT(111, shi::top(-3));

  shi::evaluate(": cs2 >r case -1 of case r@ 1 of 100 endof 2 of 200 endof "
                ">r -300 r> endcase endof -2 of case r@ 1 of -99 endof >r "
                "-199 r> endcase endof >r 299 r> endcase r> drop ;");
  TEST_ASSERT_EQUAL_INT(4, shi::size());

  shi::evaluate("-1 1 cs2");  // 100
  TEST_ASSERT_EQUAL_INT(5, shi::size());
  TEST_ASSERT_EQUAL_INT(100, shi::top());
  TEST_ASSERT_EQUAL_INT(999, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(333, shi::top(-2));
  TEST_ASSERT_EQUAL_INT(222, shi::top(-3));
  TEST_ASSERT_EQUAL_INT(111, shi::top(-4));

  shi::evaluate("-1 2 cs2");  // 200
  TEST_ASSERT_EQUAL_INT(6, shi::size());
  TEST_ASSERT_EQUAL_INT(200, shi::top());
  TEST_ASSERT_EQUAL_INT(100, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(999, shi::top(-2));
  TEST_ASSERT_EQUAL_INT(333, shi::top(-3));
  TEST_ASSERT_EQUAL_INT(222, shi::top(-4));
  TEST_ASSERT_EQUAL_INT(111, shi::top(-5));

  shi::evaluate("-1 3 cs2");  // -300
  TEST_ASSERT_EQUAL_INT(7, shi::size());
  TEST_ASSERT_EQUAL_INT(-300, shi::top());
  TEST_ASSERT_EQUAL_INT(200, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(100, shi::top(-2));
  TEST_ASSERT_EQUAL_INT(999, shi::top(-3));
  TEST_ASSERT_EQUAL_INT(333, shi::top(-4));
  TEST_ASSERT_EQUAL_INT(222, shi::top(-5));
  TEST_ASSERT_EQUAL_INT(111, shi::top(-6));

  shi::evaluate("-2 1 cs2");  // -99
  TEST_ASSERT_EQUAL_INT(8, shi::size());
  TEST_ASSERT_EQUAL_INT(-99, shi::top());
  TEST_ASSERT_EQUAL_INT(-300, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(200, shi::top(-2));
  TEST_ASSERT_EQUAL_INT(100, shi::top(-3));
  TEST_ASSERT_EQUAL_INT(999, shi::top(-4));
  TEST_ASSERT_EQUAL_INT(333, shi::top(-5));
  TEST_ASSERT_EQUAL_INT(222, shi::top(-6));
  TEST_ASSERT_EQUAL_INT(111, shi::top(-7));

  shi::evaluate("-2 2 cs2");  // -199
  TEST_ASSERT_EQUAL_INT(9, shi::size());
  TEST_ASSERT_EQUAL_INT(-199, shi::top());
  TEST_ASSERT_EQUAL_INT(-99, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(-300, shi::top(-2));
  TEST_ASSERT_EQUAL_INT(200, shi::top(-3));
  TEST_ASSERT_EQUAL_INT(100, shi::top(-4));
  TEST_ASSERT_EQUAL_INT(999, shi::top(-5));
  TEST_ASSERT_EQUAL_INT(333, shi::top(-6));
  TEST_ASSERT_EQUAL_INT(222, shi::top(-7));
  TEST_ASSERT_EQUAL_INT(111, shi::top(-8));

  shi::evaluate("0 2 cs2");  // 299
  TEST_ASSERT_EQUAL_INT(10, shi::size());
  TEST_ASSERT_EQUAL_INT(299, shi::top());
  TEST_ASSERT_EQUAL_INT(-199, shi::top(-1));
  TEST_ASSERT_EQUAL_INT(-99, shi::top(-2));
  TEST_ASSERT_EQUAL_INT(-300, shi::top(-3));
  TEST_ASSERT_EQUAL_INT(200, shi::top(-4));
  TEST_ASSERT_EQUAL_INT(100, shi::top(-5));
  TEST_ASSERT_EQUAL_INT(999, shi::top(-6));
  TEST_ASSERT_EQUAL_INT(333, shi::top(-7));
  TEST_ASSERT_EQUAL_INT(222, shi::top(-8));
  TEST_ASSERT_EQUAL_INT(111, shi::top(-9));

  shi::clear();
}

TEST(variable) {
  shi::evaluate("variable v1");
  TEST_ASSERT_TRUE(shi::empty());

  shi::evaluate("123 v1 !");
  TEST_ASSERT_TRUE(shi::empty());

  shi::evaluate("v1 @");  // 123
  TEST_ASSERT_EQUAL_INT(1, shi::size());
  TEST_ASSERT_EQUAL_INT(123, shi::top());

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
  shi::init(0x20004000,
            0x20004000 + 0x00004000,
            0x08000000 + 0x00040000 - 0x8000,
            0x08000000 + 0x00040000);
  //  asm volatile("nop");
  //  shi_test();
  //  asm volatile("nop");
  //  shi_test_compile_to_flash();

  //  semihosting_io();

  UNITY_BEGIN();

  RUN_TEST(number);
  RUN_TEST(number_hex);
  RUN_TEST(number_bin);
  RUN_TEST(top);
  RUN_TEST(if_else_then);
  RUN_TEST(loop);
  RUN_TEST(leave);
  RUN_TEST(case_);
  RUN_TEST(variable);

  return UNITY_END();
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

  // Constant
  asm volatile("nop");
  shi::evaluate("123 constant x123");
  shi::stack_print(printf);
  shi::evaluate("x123");  // 123
  shi::stack_print(printf);
  shi::evaluate(": equ constant ;");
  shi::stack_print(printf);
  shi::evaluate("x123 equ y123");
  shi::stack_print(printf);
  shi::evaluate("y123");  // 123
  shi::stack_print(printf);
  shi::clear();

  // C variable
  asm volatile("nop");
  volatile uint32_t stars = 42;
  // equal to push(&stars) and evaluate("c-variable stars");
  shi::c_variable("stars", &stars);
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
