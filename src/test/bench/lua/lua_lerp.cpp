#include <algorithm>
#include <cassert>
#include <vector>
#include "bench.h"
#include "lerp_data.h"

extern "C" {
#include "lua/lauxlib.h"
#include "lua/lua.h"
#include "lua/lualib.h"
}

void lua_lerp(lua_State* L) {
  using std::begin;
  using std::end;
  using std::equal;
  using std::vector;

  constexpr auto n{sizeof(lerp_data.x) / sizeof(lerp_data.x[0])};

  luaL_dostring(L,
                "function lerp(x, x1, x2, y1, y2)"
                "  return y1 + ((y2 - y1) * (x - x1)) / (x2 - x1) "
                "end");

  vector<int32_t> y(n);

  START_CYC_CNT();
  for (auto i{0u}; i < n; ++i) {
    lua_getglobal(L, "lerp");
    lua_pushnumber(L, lerp_data.x[i]);
    lua_pushnumber(L, lerp_data.x1[i]);
    lua_pushnumber(L, lerp_data.x2[i]);
    lua_pushnumber(L, lerp_data.y1[i]);
    lua_pushnumber(L, lerp_data.y2[i]);
    lua_call(L, 5, 1);
    y[i] = (int)lua_tointeger(L, -1);
    lua_pop(L, 1);
  }
  STOP_CYC_CNT();

  bench_assert(equal(begin(y), end(y), &lerp_data.y[0]));
}
