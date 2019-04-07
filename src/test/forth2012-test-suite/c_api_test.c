#include <stdint.h>
#include "forth2012_test_suite.hpp"
#include "shi.hpp"

TEST(c_api) {
  {
    shi_evaluate(": c 99 ;");
    shi_evaluate("c");
    TEST_ASSERT_EQUAL(1, shi_size());
    TEST_ASSERT_EQUAL(99, shi_top_number(0));

    shi_clear();
  }

  {
    Word w = shi_tick("c");
    shi_word(w);
    TEST_ASSERT_EQUAL(1, shi_size());
    TEST_ASSERT_EQUAL(99, shi_top_number(0));

    shi_clear();
  }

  // Push and pop numbers
  {
    TEST_ASSERT_EQUAL(0, shi_size());
    shi_push_number(43);
    TEST_ASSERT_EQUAL(1, shi_size());
    TEST_ASSERT_EQUAL(43, shi_top_number(0));
    int ret = shi_pop_number();
    TEST_ASSERT_EQUAL(0, shi_size());
    TEST_ASSERT_EQUAL(43, ret);

    shi_clear();
  }

  // Push and pop double-cell numbers
  {
    uint64_t big0 = 0xAAAABBBBCCCCDDDD;
    shi_push_double(big0);
    TEST_ASSERT_EQUAL(2, shi_size());

    uint64_t big0_pop = shi_pop_double();
    TEST_ASSERT_EQUAL_UINT64(big0, big0_pop);

    uint64_t big1 = 0xCC00FFEEEEFF00CC;
    uint32_t small = 42;
    shi_push_double(big0);
    shi_push_number(42);
    shi_push_double(big1);
    TEST_ASSERT_EQUAL(5, shi_size());
    uint64_t big1_pop = shi_pop_double();
    uint32_t small_pop = shi_pop_number();
    big0_pop = shi_pop_double();
    TEST_ASSERT_EQUAL(0, shi_size());
    TEST_ASSERT_EQUAL_UINT64(big1, big1_pop);
    TEST_ASSERT_EQUAL(42, small_pop);
    TEST_ASSERT_EQUAL_UINT64(big0, big0_pop);

    shi_clear();
  }

  // Push and pop user-defined types
  {
    typedef struct {
      uint16_t a;
      char b[3];
    } S;

    S s = {1337, {'a', 'b', 'c'}};
    shi_push_struct(&s, sizeof(s));
    S s_pop;
    shi_pop_struct(&s_pop, sizeof(s_pop));
    TEST_ASSERT_EQUAL(s.a, s_pop.a);
    TEST_ASSERT_EQUAL(s.b[0], s_pop.b[0]);
    TEST_ASSERT_EQUAL(s.b[1], s_pop.b[1]);
    TEST_ASSERT_EQUAL(s.b[2], s_pop.b[2]);

    shi_clear();
  }

  {
    typedef struct {
      uint16_t a;
      char b[3];
    } __attribute__((packed)) S;

    S s = {1337, {'a', 'b', 'c'}};
    shi_push_struct(&s, sizeof(s));
    S s_pop;
    shi_pop_struct(&s_pop, sizeof(s_pop));
    TEST_ASSERT_EQUAL(s.a, s_pop.a);
    TEST_ASSERT_EQUAL(s.b[0], s_pop.b[0]);
    TEST_ASSERT_EQUAL(s.b[1], s_pop.b[1]);
    TEST_ASSERT_EQUAL(s.b[2], s_pop.b[2]);

    shi_clear();
  }

  {
    typedef struct {
      uint16_t a;
      uint32_t b;
    } S;

    S s = {65535, 16777216};
    shi_push_struct(&s, sizeof(s));
    S s_pop;
    shi_pop_struct(&s_pop, sizeof(s_pop));
    TEST_ASSERT_EQUAL(s.a, s_pop.a);
    TEST_ASSERT_EQUAL(s.b, s_pop.b);

    shi_clear();
  }

  {
    typedef struct {
      uint16_t a;
      uint32_t b;
    } __attribute__((packed)) S;
    ;

    S s = {65535, 16777216};
    shi_push_struct(&s, sizeof(s));
    S s_pop;
    shi_pop_struct(&s_pop, sizeof(s_pop));
    TEST_ASSERT_EQUAL(s.a, s_pop.a);
    TEST_ASSERT_EQUAL(s.b, s_pop.b);

    shi_clear();
  }

  {
    typedef struct {
      char a;
      uint16_t b;
      uint32_t c;
    } S;

    S s = {'a', 1337, 16777216};
    shi_push_struct(&s, sizeof(s));
    S s_pop;
    shi_pop_struct(&s_pop, sizeof(s_pop));
    TEST_ASSERT_EQUAL(s.a, s_pop.a);
    TEST_ASSERT_EQUAL(s.b, s_pop.b);
    TEST_ASSERT_EQUAL(s.c, s_pop.c);

    shi_clear();
  }

  {
    typedef struct {
      char a;
      uint16_t b;
      uint32_t c;
    } __attribute__((packed)) S;

    S s = {'a', 1337, 16777216};
    shi_push_struct(&s, sizeof(s));
    S s_pop;
    shi_pop_struct(&s_pop, sizeof(s_pop));
    TEST_ASSERT_EQUAL(s.a, s_pop.a);
    TEST_ASSERT_EQUAL(s.b, s_pop.b);
    TEST_ASSERT_EQUAL(s.c, s_pop.c);

    shi_clear();
  }

  // Top
  {
    uint64_t big = 0xAAAABBBBCCCCDDDD;
    shi_push_double(big);
    uint64_t big_top = shi_top_double(0);
    TEST_ASSERT_EQUAL(2, shi_size());
    TEST_ASSERT_EQUAL_UINT64(big, big_top);

    shi_clear();
  }

  {
    uint64_t big = 0xAAAABBBBCCCCDDDD;
    shi_push_double(big);
    shi_push_number(100);
    uint64_t big_top = shi_top_double(1);
    TEST_ASSERT_EQUAL(3, shi_size());
    TEST_ASSERT_EQUAL_UINT64(big, big_top);

    shi_clear();
  }

  {
    uint64_t big = 0xAAAABBBBCCCCDDDD;
    shi_push_number(100);
    shi_push_number(101);
    shi_push_double(big);
    shi_push_number(102);
    shi_push_number(103);
    uint64_t big_top = shi_top_double(2);
    TEST_ASSERT_EQUAL(6, shi_size());
    TEST_ASSERT_EQUAL_UINT64(big, big_top);
    TEST_ASSERT_EQUAL(102, shi_top_number(1));

    shi_clear();
  }

  {
    typedef struct {
      char a;
      uint16_t b;
      uint32_t c;
    } S;

    S s = {'a', 1337, 16777216};
    shi_push_struct(&s, sizeof(s));
    shi_push_number(100);
    size_t size_before = shi_size();
    S* s_top;
    shi_top_struct(&s_top, 1);
    TEST_ASSERT_EQUAL(size_before, shi_size());
    TEST_ASSERT_EQUAL(s.a, s_top->a);
    TEST_ASSERT_EQUAL(s.b, s_top->b);
    TEST_ASSERT_EQUAL(s.c, s_top->c);

    shi_clear();
  }
}
