#include "forth2012_test_suite.hpp"

using shi::operator""_s, shi::operator""_w;

TEST(cpp_api) {
  using std::get, std::tuple;

  // Word and _w literal
  {
    // ( -- 7 )
    ": seven 7 ;"_s;

    // Create instance of word
    auto seven{shi::Word("seven")};
    TEST_ASSERT_EQUAL(0, shi::size());

    // Call word
    seven();
    TEST_ASSERT_EQUAL(1, shi::size());
    TEST_ASSERT_EQUAL(7, shi::top());

    // Same call using _w literal
    "seven"_w();
    TEST_ASSERT_EQUAL(2, shi::size());
    TEST_ASSERT_EQUAL(7, shi::top());
    TEST_ASSERT_EQUAL(7, shi::top(1));

    // Using implicit conversion to retrieve value
    int32_t ret{seven()};
    TEST_ASSERT_EQUAL(2, shi::size());
    TEST_ASSERT_EQUAL(7, ret);

    // Also works with _w literal
    ret = "seven"_w();
    TEST_ASSERT_EQUAL(2, shi::size());
    TEST_ASSERT_EQUAL(7, ret);

    // Pass multiple parameters
    ": cpp_sum + ;"_s;
    int32_t sum{"cpp_sum"_w(42, 7)};
    TEST_ASSERT_EQUAL(2, shi::size());
    TEST_ASSERT_EQUAL(42 + 7, sum);

    // Multiple returns
    ": range 0 do i loop ;"_s;
    tuple<uint32_t, uint32_t, uint32_t, uint32_t> t{"range"_w(4)};
    TEST_ASSERT_EQUAL(2, shi::size());
    TEST_ASSERT_EQUAL(3, get<0>(t));
    TEST_ASSERT_EQUAL(2, get<1>(t));
    TEST_ASSERT_EQUAL(1, get<2>(t));
    TEST_ASSERT_EQUAL(0, get<3>(t));

    shi::clear();
  }

  // Push and pop double-cell values
  {
    uint64_t big0{0xAAAABBBBCCCCDDDD};
    shi::push(big0);
    TEST_ASSERT_EQUAL(2, shi::size());

    uint64_t big0_pop{shi::pop<uint64_t>()};
    TEST_ASSERT_EQUAL_UINT64(big0, big0_pop);

    uint64_t big1{0xCC00FFEEEEFF00CC};
    uint32_t small{42};
    shi::push(big0, 42, big1);
    TEST_ASSERT_EQUAL(5, shi::size());

    auto t{shi::pop<uint64_t, uint32_t, uint64_t>()};
    TEST_ASSERT_EQUAL(0, shi::size());
    TEST_ASSERT_EQUAL_UINT64(big1, get<0>(t));
    TEST_ASSERT_EQUAL(42, get<1>(t));
    TEST_ASSERT_EQUAL_UINT64(big0, get<2>(t));

    shi::clear();
  }

  {
    struct S {
      uint16_t a;
      char b[3];
    };

    S s{1337, {'a', 'b', 'c'}};
    shi::push(s);
    auto s_pop{shi::pop<S>()};
    TEST_ASSERT_EQUAL(s.a, s_pop.a);
    TEST_ASSERT_EQUAL(s.b[0], s_pop.b[0]);
    TEST_ASSERT_EQUAL(s.b[1], s_pop.b[1]);
    TEST_ASSERT_EQUAL(s.b[2], s_pop.b[2]);

    shi::clear();
  }

  {
    struct [[gnu::packed]] S {
      uint16_t a;
      char b[3];
    };

    S s{1337, {'a', 'b', 'c'}};
    shi::push(s);
    auto s_pop{shi::pop<S>()};
    TEST_ASSERT_EQUAL(s.a, s_pop.a);
    TEST_ASSERT_EQUAL(s.b[0], s_pop.b[0]);
    TEST_ASSERT_EQUAL(s.b[1], s_pop.b[1]);
    TEST_ASSERT_EQUAL(s.b[2], s_pop.b[2]);

    shi::clear();
  }

  {
    struct S {
      uint16_t a;
      uint32_t b;
    };

    S s{65535, 16777216};
    shi::push(s);
    auto s_pop{shi::pop<S>()};
    TEST_ASSERT_EQUAL(s.a, s_pop.a);
    TEST_ASSERT_EQUAL(s.b, s_pop.b);

    shi::clear();
  }

  {
    struct [[gnu::packed]] S {
      uint16_t a;
      uint32_t b;
    };

    S s{65535, 16777216};
    shi::push(s);
    auto s_pop{shi::pop<S>()};
    TEST_ASSERT_EQUAL(s.a, s_pop.a);
    TEST_ASSERT_EQUAL(s.b, s_pop.b);

    shi::clear();
  }

  {
    struct S {
      char a;
      uint16_t b;
      uint32_t c;
    };

    S s{'a', 1337, 16777216};
    shi::push(s);
    auto s_pop{shi::pop<S>()};
    TEST_ASSERT_EQUAL(s.a, s_pop.a);
    TEST_ASSERT_EQUAL(s.b, s_pop.b);
    TEST_ASSERT_EQUAL(s.c, s_pop.c);

    shi::clear();
  }

  {
    struct [[gnu::packed]] S {
      char a;
      uint16_t b;
      uint32_t c;
    };

    S s{'a', 1337, 16777216};
    shi::push(s);
    auto s_pop{shi::pop<S>()};
    TEST_ASSERT_EQUAL(s.a, s_pop.a);
    TEST_ASSERT_EQUAL(s.b, s_pop.b);
    TEST_ASSERT_EQUAL(s.c, s_pop.c);

    shi::clear();
  }

  {
    uint64_t big{0xAAAABBBBCCCCDDDD};
    shi::push(big);
    auto big_top{shi::top<uint64_t>()};
    TEST_ASSERT_EQUAL_UINT64(big, big_top);
  }

  {
    uint64_t big{0xAAAABBBBCCCCDDDD};
    shi::push(big, 100);
    auto big_top{shi::top<uint64_t>(1)};
    TEST_ASSERT_EQUAL_UINT64(big, big_top);
  }

  {
    uint64_t big{0xAAAABBBBCCCCDDDD};
    shi::push(100, 101, big, 100, 101);
    auto big_top{shi::top<uint64_t>(2)};
    TEST_ASSERT_EQUAL_UINT64(big, big_top);
  }
}
