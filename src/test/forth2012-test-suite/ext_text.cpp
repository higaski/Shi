#include "dwt_cyccnt.h"
#include "forth2012_test_suite.hpp"

using namespace shi::literals;

TEST(to_text_q__to_data_q) {
  ">data?"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  ">text?"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());

  ">text"_s;

  ">data?"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());
  ">text?"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());

  ">data"_s;

  ">data?"_s;
  TEST_ASSERT_EQUAL(TRUE, shi::top());
  ">text?"_s;
  TEST_ASSERT_EQUAL(FALSE, shi::top());

  shi::clear();
}

TEST(to_text__to_data) {
  // This test only runs on real hardware
  // It's easy to check if we are running on actual hardware by reading the cyc
  // cnt register (still 0 means we're running QEMU)
  if (!DWT_CYCCNT_GET())
    return;

  if (!shi::tick("T1")) {
    ">text"_s;
    ": T1 111 ;"_s;
    ">data"_s;
  }

  "T1"_s;
  TEST_ASSERT_EQUAL(111, shi::top());

  if (!shi::tick("T2")) {
    ">text"_s;
    ": T2 222 ;"_s;
    ": T3 T2 1+ ;"_s;
    ">data"_s;
  }

  "T2"_s;
  TEST_ASSERT_EQUAL(222, shi::top());

  "T3"_s;
  TEST_ASSERT_EQUAL(223, shi::top());
  TEST_ASSERT_EQUAL(222, shi::top(1));

  if (!shi::tick("T4")) {
    ">text"_s;
    ": T4 444 ; inline"_s;
    ": T5 T4 2 * ;"_s;
    ">data"_s;
  }

  "T4"_s;
  TEST_ASSERT_EQUAL(444, shi::top());

  "T5"_s;
  TEST_ASSERT_EQUAL(888, shi::top());
  TEST_ASSERT_EQUAL(444, shi::top(1));

  ": T6 42 + ;"_s;

  if (!shi::tick("T7")) {
    ">text"_s;
    ": T7 T6 ;"_s;
    ">data"_s;
  }

  "100 T7"_s;
  TEST_ASSERT_EQUAL(142, shi::top());

  shi::clear();
}
