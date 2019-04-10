#include "shi.hpp"

void c_quick_n_dirty() {
  // Running code
  shi_evaluate("13 29 +");
  printf("size:%d top:%d\n", shi_size(), shi_top_number(0));

  // Creating a new definition called "mean" and then call it through Word
  shi_evaluate(": mean + 2 / ;");
  Word mean = shi_tick("mean");
  shi_push_number(100);
  shi_push_number(200);
  shi_word(mean);
  int32_t retval = shi_pop_number();
  printf("mean(100, 200) = %d\n", retval);

  // Pushing values onto the stack
  int64_t big = 6812712319812946239;
  shi_push_number(111);
  shi_push_double(big);
  shi_push_number(222);
  printf("size:%d top:%d\n", shi_size(), shi_top_number(0));

  // Getting values from the stack without popping can be done using top
  // top takes a "cell-sized" offset to refer to elements further down the stack
  int64_t big_top = shi_top_double(1);
  printf("big equal big_top ? %s\n", big == big_top ? "yes" : "no");

  // Popping values
  int32_t small_pop = shi_pop_number();
  printf("size:%d\n", shi_size());
  int64_t big_pop = shi_pop_double();
  printf("size:%d\n", shi_size());

  // Clear the stack
  shi_clear();
  printf("size:%d top:%c\n", shi_size(), shi_top_number(0));

  // Pushing user-defined types onto the stack
  typedef struct {
    char c[3];
    int32_t i;
  } S;
  S s = {{'x', 'y', 'z'}, 1000};
  shi_push_struct(&s, sizeof(s));
  shi_push_number(111);
  shi_push_number(222);
  shi_push_number(333);

  // User-defined types on the stack can be accessed by pointer
  S* s_top;
  shi_top_struct(&s_top, 3);
  printf("s_ref c[0]:%c c[1]:%c c[2]:%c i:%d\n",
         s_top->c[0],
         s_top->c[1],
         s_top->c[2],
         s_top->i);

  // Creating a definition which refers to a C variable
  int32_t moore_birthday = 1938;
  shi_variable(&moore_birthday, "moore_bday");
  shi_evaluate("2019 moore_bday @ -");
  printf("Charels H. Moore was born %d years ago\n", shi_top_number(0));
}
