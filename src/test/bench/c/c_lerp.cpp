#include <algorithm>
#include <cstdio>
#include <vector>
#include "bench.h"
#include "lerp_data.h"

namespace {

[[gnu::noinline]] int lerp(int x, int x1, int x2, int y1, int y2) {
  return y1 + ((y2 - y1) * (x - x1)) / (x2 - x1);
}

}  // namespace

int c_lerp() {
  using std::equal, std::vector;

  constexpr auto n{sizeof(lerp_data.x) / sizeof(lerp_data.x[0])};

  vector<int32_t> y(n);

  START_CYC_CNT();
  for (auto i{0u}; i < n; ++i)
    y[i] = lerp(lerp_data.x[i],
                lerp_data.x1[i],
                lerp_data.x2[i],
                lerp_data.y1[i],
                lerp_data.y2[i]);
  STOP_CYC_CNT();

  return equal(begin(y), end(y), &lerp_data.y[0]) ? 0 : 1;
}
