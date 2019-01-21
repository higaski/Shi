#include "dwt_cyccnt.h"
#include "lua/lauxlib.h"
#include "lua/lua.h"
#include "lua/lualib.h"
#include "stm32f4xx_hal.h"

void test_performance_lua() {
  // initialize Lua interpreter
  lua_State* L = luaL_newstate();

  // load Lua base libraries (print / math / etc)
  luaL_openlibs(L);

  luaL_dostring(L, "function foo(n) return n*2 end");

  __disable_irq();
  uint32_t it1, it2;
  DWT_CYCCNT_EN();
  it1 = DWT_CYCCNT_GET();

  lua_getglobal(L, "foo");
  lua_pushnumber(L, 13);
  lua_call(L, 1, 1);
  int result = (int)lua_tointeger(L, -1);
  lua_pop(L, 1);

  it2 = DWT_CYCCNT_GET() - it1;
  DWT_CYCCNT_DIS();
  __enable_irq();
  printf("result: %d\n", result);
  printf("%d\n", it2);
  HAL_Delay(1000);

  lua_close(L);
}
