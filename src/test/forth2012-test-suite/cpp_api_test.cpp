#include "forth2012_test_suite.hpp"

using shi::operator""_s, shi::operator""_w;

TEST(cpp_api_word) {
  using std::get, std::tuple;

  // ( -- 7 )
  ": seven 7 ;"_s;

  // Create instance of word
  auto seven{shi::word("seven")};
  TEST_ASSERT_EQUAL(0, shi::size());

  // Call
  seven();
  TEST_ASSERT_EQUAL(1, shi::size());
  TEST_ASSERT_EQUAL(7, shi::top());

  // Same call using literal _w
  "seven"_w();
  TEST_ASSERT_EQUAL(2, shi::size());
  TEST_ASSERT_EQUAL(7, shi::top());
  TEST_ASSERT_EQUAL(7, shi::top(1));

  // Using implicit conversion
  int32_t ret{seven()};
  TEST_ASSERT_EQUAL(2, shi::size());
  TEST_ASSERT_EQUAL(7, ret);

  // Also works with literal _w
  ret = "seven"_w();
  TEST_ASSERT_EQUAL(2, shi::size());
  TEST_ASSERT_EQUAL(7, ret);

  // Pass multiple parameters
  ": sum2 + ;"_s;
  int32_t sum{"sum2"_w(42, 7)};
  TEST_ASSERT_EQUAL(2, shi::size());
  TEST_ASSERT_EQUAL(42 + 7, sum);

  // Return multiple parameters
  ": range 0 do i loop ;"_s;
  tuple<uint32_t, uint32_t, uint32_t, uint32_t> t{"range"_w(4)};
  TEST_ASSERT_EQUAL(2, shi::size());
  TEST_ASSERT_EQUAL(3, get<0>(t));
  TEST_ASSERT_EQUAL(2, get<1>(t));
  TEST_ASSERT_EQUAL(1, get<2>(t));
  TEST_ASSERT_EQUAL(0, get<3>(t));

  shi::clear();
}

TEST(cpp_push_8) {
  using std::get;

  uint64_t big0{0xAAAABBBBCCCCDDDD};
  shi::push(big0);
  TEST_ASSERT_EQUAL(2, shi::size());

  uint64_t big0_pop{shi::pop<uint64_t>()};
  TEST_ASSERT_EQUAL_UINT64(0xAAAABBBBCCCCDDDD, big0_pop);

  uint64_t big1{0xCC00FFEEEEFF00CC};
  uint32_t small{42};
  shi::push(big0, 42, big1);
  TEST_ASSERT_EQUAL(5, shi::size());

  auto t{shi::pop<uint64_t, uint32_t, uint64_t>()};
  TEST_ASSERT_EQUAL(0, shi::size());
  TEST_ASSERT_EQUAL_UINT64(0xCC00FFEEEEFF00CC, get<0>(t));
  TEST_ASSERT_EQUAL(42, get<1>(t));
  TEST_ASSERT_EQUAL_UINT64(0xAAAABBBBCCCCDDDD, get<2>(t));

  shi::clear();
}
