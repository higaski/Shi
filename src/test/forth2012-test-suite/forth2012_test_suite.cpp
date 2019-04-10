#include "forth2012_test_suite.hpp"
#include "kB_literal.hpp"
#include "main.h"

// Core
TEST(basic_assumptions);
TEST(invert__and__or__xor);
TEST(two_times__two_div__lshift__rshit);
TEST(zero_equal__equal__zero_less__less__more__u_less__min__max);
TEST(__2drop__2dup__2over__2swap_q_dup__depth__drop__dup__over__rot__swap);
TEST(to_r__from_r__r_fetch);
TEST(plus__minus__one_plus__one_minus__abs__negate);
TEST(s_to_d__m_times__um_times);
TEST(
    fm_div_mod__sm_div_rem__um_div_mod__times_div__times_div_mod__div__div_mod__mod);
TEST(
    here__comma__fetch__store__cell_plus__cells__c_comma__c_fetch__c_store__chars__two_fetch__two_store__align__aligned__plus_store__allot);
TEST(
    tick__bracket_tick__find__execute__immediate__count__literal__postpone__state);
TEST(if__else__then__begin__while__repeat__until__recurse);
TEST(do__loop__plus_loop__i__j__unloop__leave__exit);
TEST(colon__semicolon__constant__variable__create__does__to_body);
TEST(source__to_in__word);
TEST(num_start__num__num_s__hold__sign__base__to_number__hex__decimal);

// Core extension
TEST(ne__u_more);
TEST(zero_ne__zero_more);
TEST(nip__tuck__roll__pick);
TEST(two_to_r__two_r_fetch__two_r_from);
TEST(hex);
TEST(within);
TEST(unused);
TEST(again);
TEST(marker);
TEST(q_do);
TEST(buffer);
TEST(value__to);
TEST(case__of__endof__endcase);

//
TEST(corner_cases_loop);
TEST(corner_cases_case);
TEST(corner_cases_while);

// C/C++ api
TEST(c_api);
TEST(cpp_api);

namespace {

alignas(4) std::array<uint8_t, 32_kB> data{};

}  // namespace

int forth2012_test_suite() {
  shi::init({.data_begin = reinterpret_cast<uint32_t>(begin(data)),
             .data_end = reinterpret_cast<uint32_t>(end(data)),
             .text_begin = FLASH_END - 32_kB,
             .text_end = FLASH_END});

  UNITY_BEGIN();

  RUN_TEST(basic_assumptions);
  RUN_TEST(invert__and__or__xor);
  RUN_TEST(two_times__two_div__lshift__rshit);
  RUN_TEST(zero_equal__equal__zero_less__less__more__u_less__min__max);
  RUN_TEST(
      __2drop__2dup__2over__2swap_q_dup__depth__drop__dup__over__rot__swap);
  RUN_TEST(to_r__from_r__r_fetch);
  RUN_TEST(plus__minus__one_plus__one_minus__abs__negate);
  RUN_TEST(s_to_d__m_times__um_times);
  RUN_TEST(
      fm_div_mod__sm_div_rem__um_div_mod__times_div__times_div_mod__div__div_mod__mod);
  RUN_TEST(
      here__comma__fetch__store__cell_plus__cells__c_comma__c_fetch__c_store__chars__two_fetch__two_store__align__aligned__plus_store__allot);
  RUN_TEST(
      tick__bracket_tick__find__execute__immediate__count__literal__postpone__state);
  RUN_TEST(if__else__then__begin__while__repeat__until__recurse);
  RUN_TEST(do__loop__plus_loop__i__j__unloop__leave__exit);
  RUN_TEST(colon__semicolon__constant__variable__create__does__to_body);
  RUN_TEST(source__to_in__word);
  RUN_TEST(num_start__num__num_s__hold__sign__base__to_number__hex__decimal);

  RUN_TEST(ne__u_more);
  RUN_TEST(zero_ne__zero_more);
  RUN_TEST(nip__tuck__roll__pick);
  RUN_TEST(two_to_r__two_r_fetch__two_r_from);
  RUN_TEST(hex);
  RUN_TEST(within);
  RUN_TEST(unused);
  RUN_TEST(again);
  RUN_TEST(marker);
  RUN_TEST(q_do);
  RUN_TEST(buffer);
  RUN_TEST(value__to);
  RUN_TEST(case__of__endof__endcase);

  RUN_TEST(corner_cases_loop);
  RUN_TEST(corner_cases_case);
  RUN_TEST(corner_cases_while);

  RUN_TEST(c_api);
  RUN_TEST(cpp_api);

  return UNITY_END();
}
