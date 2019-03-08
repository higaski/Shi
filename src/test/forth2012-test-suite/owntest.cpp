#include "forth2012_test_suite.hpp"

using shi::operator""_fs;

TEST(own_leave) {
  ": GD7 do i 2 = if 2 leave then i -2 = if -2 leave then loop ;"_fs;
  TEST_ASSERT_EQUAL(0, shi::size());
  "4 1 GD7"_fs;
  TEST_ASSERT_EQUAL(1, shi::size());
  TEST_ASSERT_EQUAL(2, shi::top());
  "-1 -5 GD7"_fs;
  TEST_ASSERT_EQUAL(2, shi::size());
  TEST_ASSERT_EQUAL(-2, shi::top());
  TEST_ASSERT_EQUAL(2, shi::top(-1));

  shi::clear();
}
