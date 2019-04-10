#include "bench.h"

extern "C" {
#include "lua/lauxlib.h"
#include "lua/lua.h"
#include "lua/lualib.h"
}

int lua_acker(lua_State* L) {
  luaL_dostring(L,
                "function ack(M,N)"
                "  if M==0 then return N+1 end"
                "  if N==0 then return ack(M-1,1) end"
                "  return ack(M-1,ack(M, N-1))"
                "end");

  START_CYC_CNT();
  lua_getglobal(L, "ack");
  lua_pushnumber(L, 3);
  lua_pushnumber(L, 2);
  lua_call(L, 2, 1);
  int retval{(int)lua_tointeger(L, -1)};
  lua_pop(L, 1);
  STOP_CYC_CNT();

  return retval == 29 ? 0 : 1;
}
