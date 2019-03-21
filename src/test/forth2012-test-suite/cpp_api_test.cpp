#include "forth2012_test_suite.hpp"

using shi::operator""_s;
using shi::operator""_w;

TEST(cpp_api_word) {
  ": seven 7 ;"_s;
  auto seven{shi::word("seven")};
  TEST_ASSERT_EQUAL(0, shi::size());

  seven();
  TEST_ASSERT_EQUAL(1, shi::size());
  TEST_ASSERT_EQUAL(7, shi::top());

  "seven"_w();
  TEST_ASSERT_EQUAL(2, shi::size());
  TEST_ASSERT_EQUAL(7, shi::top());
  TEST_ASSERT_EQUAL(7, shi::top(1));

  shi::clear();
}
