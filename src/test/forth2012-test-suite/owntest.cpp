#include "forth2012_test_suite.hpp"

using shi::operator""_fs;

TEST(own_loop) {
  ": GD7 do i 2 = if 2 leave then i -2 = if -2 leave then loop ;"_fs;
  TEST_ASSERT_EQUAL(0, shi::size());
  "4 1 GD7"_fs;
  TEST_ASSERT_EQUAL(1, shi::size());
  TEST_ASSERT_EQUAL(2, shi::top());
  "-1 -5 GD7"_fs;
  TEST_ASSERT_EQUAL(2, shi::size());
  TEST_ASSERT_EQUAL(-2, shi::top());
  TEST_ASSERT_EQUAL(2, shi::top(1));

  shi::clear();
}

TEST(own_case) {
  ": CS8 do i case 1 of 111 endof 2 of 222 endof 3 of 333 endof endcase loop ;"_fs;
  "4 1 CS8"_fs;
  TEST_ASSERT_EQUAL(3, shi::size());
  TEST_ASSERT_EQUAL(333, shi::top());
  TEST_ASSERT_EQUAL(222, shi::top(1));
  TEST_ASSERT_EQUAL(111, shi::top(2));
  "3 1 CS8"_fs;
  TEST_ASSERT_EQUAL(5, shi::size());
  TEST_ASSERT_EQUAL(222, shi::top());
  TEST_ASSERT_EQUAL(111, shi::top(1));
  "4 2 CS8"_fs;
  TEST_ASSERT_EQUAL(7, shi::size());
  TEST_ASSERT_EQUAL(333, shi::top());
  TEST_ASSERT_EQUAL(222, shi::top(1));

  ": CS9 do i case 1 of leave endof 2 of 222 endof 3 of 333 endof endcase loop ;"_fs;
  "4 1 CS9"_fs;
  TEST_ASSERT_EQUAL(7, shi::size());
  "3 1 CS9"_fs;
  TEST_ASSERT_EQUAL(7, shi::size());
  "4 2 CS9"_fs;
  TEST_ASSERT_EQUAL(9, shi::size());
  TEST_ASSERT_EQUAL(333, shi::top());
  TEST_ASSERT_EQUAL(222, shi::top(1));

  ": CS10 case 1 of 111 endof 2 of 4 1 do i loop endof 3 of 333 endof endcase ;"_fs;
  "1 CS10"_fs;
  TEST_ASSERT_EQUAL(10, shi::size());
  TEST_ASSERT_EQUAL(111, shi::top());
  "2 CS10"_fs;
  TEST_ASSERT_EQUAL(13, shi::size());
  TEST_ASSERT_EQUAL(3, shi::top());
  TEST_ASSERT_EQUAL(2, shi::top(1));
  TEST_ASSERT_EQUAL(1, shi::top(2));
  "3 CS10"_fs;
  TEST_ASSERT_EQUAL(14, shi::size());
  TEST_ASSERT_EQUAL(333, shi::top());

  shi::clear();
}

TEST(own_while) {
  ": GI7 10 begin 1- dup while 1- dup while 1- dup 0= until then then ;"_fs;
  TEST_ASSERT_EQUAL(0, shi::size());
  "1 GI7"_fs;
  TEST_ASSERT_EQUAL(2, shi::size());
  TEST_ASSERT_EQUAL(0, shi::top());
  TEST_ASSERT_EQUAL(1, shi::top(1));
  "2 GI7"_fs;
  TEST_ASSERT_EQUAL(4, shi::size());
  TEST_ASSERT_EQUAL(0, shi::top());
  TEST_ASSERT_EQUAL(2, shi::top(1));
  "333 GI7"_fs;
  TEST_ASSERT_EQUAL(6, shi::size());
  TEST_ASSERT_EQUAL(0, shi::top());
  TEST_ASSERT_EQUAL(333, shi::top(1));

  shi::clear();
}
