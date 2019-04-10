#include "bench.h"

[[gnu::noinline]] int ackermann(int m, int n) {
  if (!m)
    return n + 1;
  if (!n)
    return ackermann(m - 1, 1);
  return ackermann(m - 1, ackermann(m, n - 1));
}

int c_acker() {
  volatile auto m{3}, n{2};
  START_CYC_CNT();
  auto retval{ackermann(m, n)};
  STOP_CYC_CNT();

  return retval == 29 ? 0 : 1;
}
