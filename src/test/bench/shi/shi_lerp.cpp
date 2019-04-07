#include <cstdio>
#include <vector>
#include "bench.h"
#include "lerp_data.h"
#include "shi.hpp"

namespace {

void shi_lerp_define() {
  using shi::operator""_s;

  // lerp ( x x1 x2 y1 y2 -- y )
  ": lerp over - 4 roll 4 pick - * 2swap swap - / + ;"_s;

  // I thought one without roll would be faster... but it ain't really
  // ": lerp over - 2>r over 2swap - r> * r> swap 2swap - / + ;"_s;
}

constexpr auto n{sizeof(lerp_data.x) / sizeof(lerp_data.x[0])};

}  // namespace

void shi_lerp_searching_word() {
  using shi::operator""_s, std::begin, std::end, std::equal, std::vector;

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

void shi_lerp_storing_word() {
  using shi::operator""_w, std::begin, std::end, std::equal, std::vector;

  vector<int32_t> y(n);

  START_CYC_CNT();
  for (auto i{0u}; i < n; ++i) {
    y[i] = "lerp"_w(lerp_data.x[i],
                    lerp_data.x1[i],
                    lerp_data.x2[i],
                    lerp_data.y1[i],
                    lerp_data.y2[i]);
  }
  STOP_CYC_CNT();

  bench_assert(equal(begin(y), end(y), &lerp_data.y[0]));
}

void shi_lerp() {
  shi_lerp_define();
  shi_lerp_searching_word();
  shi_lerp_storing_word();
}
