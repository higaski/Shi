extern "C" {
#include "lua/lauxlib.h"
#include "lua/lua.h"
#include "lua/lualib.h"
}

int lua_acker(lua_State* L);
int lua_count_primes_by_trial_division(lua_State* L);
int lua_lerp(lua_State* L);

int lua_bench() {
  lua_State* L{luaL_newstate()};  // Initialize Lua interpreter

  // luaL_openlibs(L); // Load Lua base libraries (print / math / etc)

  int retval{};

  asm volatile("nop");
  retval |= lua_acker(L);
  asm volatile("nop");
  retval |= lua_count_primes_by_trial_division(L);  // 15.96s
  asm volatile("nop");
  retval |= lua_lerp(L);  // 20.7ms
  asm volatile("nop");

  lua_close(L);

  return retval;
}
