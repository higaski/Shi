#include "shi.hpp"

void cpp_quick_n_dirty() {
  using namespace shi::literals;

  // Running code
  shi::evaluate("13 29 +");
  printf("size:%d top:%d\n", shi::size(), shi::top());

  // Same but using literal _s
  "1 -"_s;
  printf("size:%d top:%d\n", shi::size(), shi::top());

  // Creating a new definition called "mean" and then call it through Word
  ": mean + 2 / ;"_s;
  shi::Word mean{"mean"};
  int32_t retval{mean(100, 200)};
  printf("mean(100, 200) = %d\n", retval);

  // Same but using literal _w
  retval = "mean"_w(100, 50);
  printf("mean(100, 50) = %d\n", retval);

  // Getting multiple return values by using std::tuple
  ": range 0 do i loop ;"_s;
  std::tuple<int32_t, int32_t, int32_t, int32_t> t{"range"_w(4)};
  printf("range: %d %d %d %d\n",
         std::get<0>(t),
         std::get<1>(t),
         std::get<2>(t),
         std::get<3>(t));

  // Pushing values onto the stack
  int64_t big{6'812'712'319'812'946'239};
  shi::push(111, big, 222);
  printf("size:%d top:%d\n", shi::size(), shi::top());

  // Getting values from the stack without popping can be done using top
  // top takes a "cell-sized" offset to refer to elements further down the stack
  int64_t big_top{shi::top<int64_t>(1)};
  printf("big equal big_top ? %s\n", big == big_top ? "yes" : "no");

  // Popping values
  int32_t small_pop{shi::pop()};
  printf("size:%d\n", shi::size());
  int64_t big_pop{shi::pop<int64_t>()};
  printf("size:%d\n", shi::size());

  // Clear the stack
  shi::clear();
  printf("size:%d top:%c\n", shi::size(), shi::top());

  // Pushing user-defined types onto the stack
  struct S {
    char c[3];
    int32_t i;
  };
  S s{{'x', 'y', 'z'}, 1000};
  shi::push(s, 111, 222, 333);

  // User-defined types on the stack can be accessed by reference
  S& s_top{shi::top<S>(3)};
  printf("s_top c[0]:%c c[1]:%c c[2]:%c i:%d\n",
         s_top.c[0],
         s_top.c[1],
         s_top.c[2],
         s_top.i);

  // Creating a definition which refers to a C variable
  int32_t moore_birthday{1938};
  shi::variable(&moore_birthday, "moore_bday");
  "2019 moore_bday @ -"_s;
  printf("Charels H. Moore was born %d years ago\n", shi::top());

  // Same but using literal _v
  uint8_t moore_age;
  "moore_age"_v(&moore_age);
  "moore_age c!"_s;
  printf("Moore's age equal 81 ? %s\n", moore_age == 81 ? "yes" : "no");
}
