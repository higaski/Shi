#include <cstdio>
#include <vector>
#include "bench.h"
#include "lerp_data.h"
#include "shi.hpp"

using shi::operator""_s;

void shi_lerp() {
  using std::begin;
  using std::end;
  using std::equal;
  using std::vector;

  constexpr auto n{sizeof(lerp_data.x) / sizeof(lerp_data.x[0])};

  // lerp ( x x1 x2 y1 y2 -- y )
  ": lerp over - 4 roll 4 pick - * 2swap swap - / + ;"_s;

  vector<int32_t> y(n);

  START_CYC_CNT();
  for (auto i{0u}; i < n; ++i) {
    shi::push(lerp_data.x[i]);
    shi::push(lerp_data.x1[i]);
    shi::push(lerp_data.x2[i]);
    shi::push(lerp_data.y1[i]);
    shi::push(lerp_data.y2[i]);
    "lerp"_s;
    y[i] = shi::top();
    shi::pop();
  }
  STOP_CYC_CNT();

  bench_assert(equal(begin(y), end(y), &lerp_data.y[0]));
}
