#include "mpapi.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void mp_dostring(const char* src, mp_parse_input_kind_t input_kind) {
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

mp_obj_t mp_compilestring(const char* src, mp_parse_input_kind_t input_kind) {
  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {
    mp_lexer_t* lex =
        mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
    qstr source_name = lex->source_name;
    mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);
    mp_obj_t module_fun =
        mp_compile(&parse_tree, source_name, MP_EMIT_OPT_NONE, true);
    nlr_pop();
    return module_fun;
  } else {
    // uncaught exception
    mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
  }
  return mp_const_none;
}

void mp_dobytecode(mp_obj_t module_fun) {
  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {
    mp_call_function_0(module_fun);
    nlr_pop();
  } else {
    // uncaught exception
    mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
  }
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
