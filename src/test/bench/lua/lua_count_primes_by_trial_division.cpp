#include <algorithm>
#include <vector>
#include "bench.h"
#include "count_primes_by_trial_division_data.h"

extern "C" {
#include "lua/lauxlib.h"
#include "lua/lua.h"
#include "lua/lualib.h"
}

int lua_count_primes_by_trial_division(lua_State* L) {
  luaL_dostring(L,
                "function is_prime(n)"
                "  if (n%2)==0 or n<=2 then return n==2 end"
                "  p=3"
                "  while p<=n/p do"
                "    if (n%p)==0 then return false end"
                "    p=p+2"
                "  end"
                "  return true "
                "end");

  luaL_dostring(L,
                "function count_primes(n)"
                "  retval=0"
                "  for i=0,n,1 do"
                "  if is_prime(i) then retval=retval+1 end"
                "  end"
                "  return retval "
                "end ");

  START_CYC_CNT();
  lua_getglobal(L, "count_primes");
  lua_pushnumber(L, COUNT_PRIMES_TILL);
  lua_call(L, 1, 1);
  auto retval{lua_tointeger(L, -1)};
  lua_pop(L, 1);
  STOP_CYC_CNT();

  return retval == RESULT_PRIMES_TILL ? 0 : 1;
}
