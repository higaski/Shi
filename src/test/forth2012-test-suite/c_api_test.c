#include "forth2012_test_suite.hpp"
#include "shi.hpp"

TEST(c_api) {
  TEST_ASSERT_EQUAL(0, shi_size());
  shi_push_number(43);
  TEST_ASSERT_EQUAL(1, shi_size());
  TEST_ASSERT_EQUAL(43, shi_top(0));
  int ret = shi_pop_number();
  TEST_ASSERT_EQUAL(0, shi_size());
  TEST_ASSERT_EQUAL(43, ret);

  shi_evaluate(": c 99 ;");
  shi_evaluate("c");
  TEST_ASSERT_EQUAL(1, shi_size());
  TEST_ASSERT_EQUAL(99, shi_top(0));

  Word w = shi_tick("c");
  shi_word(w);
  TEST_ASSERT_EQUAL(2, shi_size());
  TEST_ASSERT_EQUAL(99, shi_top(0));
  TEST_ASSERT_EQUAL(99, shi_top(1));

  shi_clear();
}
