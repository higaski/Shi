extern "C" {
#include "lua/lauxlib.h"
#include "lua/lua.h"
#include "lua/lualib.h"
}

void lua_count_primes_by_trial_division(lua_State* L);
void lua_lerp(lua_State* L);

int lua_bench() {
  lua_State* L{luaL_newstate()};  // Initialize Lua interpreter
  // luaL_openlibs(L); // Load Lua base libraries (print / math / etc)

  lua_count_primes_by_trial_division(L);
  lua_lerp(L);

  lua_close(L);

  return 0;
}
