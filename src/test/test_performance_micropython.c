#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "py/compile.h"
#include "py/mperrno.h"
#include "py/repl.h"
#include "py/runtime.h"

/* https://github.com/dhylands/micropython/blob/c-sample2/stmhal/c_sample.c
 * https://forum.micropython.org/viewtopic.php?f=2&t=1411&start=10
 * https://micropython-dev-docs.readthedocs.io/en/latest/adding-module.html
 */

void do_str(const char* src, mp_parse_input_kind_t input_kind) {
  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {
    mp_lexer_t* lex =
        mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
    qstr source_name = lex->source_name;
    mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);
    mp_obj_t module_fun =
        mp_compile(&parse_tree, source_name, MP_EMIT_OPT_NONE, true);
    mp_call_function_0(module_fun);
    nlr_pop();
  } else {
    // uncaught exception
    mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
  }
}

void test_performance_micropython() {
  mp_init();
  do_str("print('hello world!', list(x+1 for x in range(10)), end='eol\\n')",
         MP_PARSE_SINGLE_INPUT);
  do_str("for i in range(10):\n  print(i)", MP_PARSE_FILE_INPUT);
  mp_deinit();
}

mp_lexer_t* mp_lexer_new_from_file(const char* filename) {
  mp_raise_OSError(MP_ENOENT);
}

mp_import_stat_t mp_import_stat(const char* path) {
  return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(size_t n_args,
                         const mp_obj_t* args,
                         mp_map_t* kwargs) {
  return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

void nlr_jump_fail(void* val) {
  while (1)
    ;
}