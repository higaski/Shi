#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "py/compile.h"
#include "py/mperrno.h"
#include "py/repl.h"
#include "py/runtime.h"

typedef struct {
  int32_t x, x1, x2, y1, y2;
} S;

S s = {};

STATIC mp_obj_t IC_get_struct(void) {
  return mp_obj_new_int(&s);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(IC_get_struct_obj, IC_get_struct);

STATIC const mp_map_elem_t IC_globals_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_IC)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_get_struct), (mp_obj_t)&IC_get_struct_obj},
};

STATIC MP_DEFINE_CONST_DICT(mp_module_IC_globals, IC_globals_table);

const mp_obj_module_t mp_module_IC = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t*)&mp_module_IC_globals,
};

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

mp_obj_t compile_str(const char* src, mp_parse_input_kind_t input_kind) {
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

void call_bytecode(mp_obj_t module_fun) {
  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {
    mp_call_function_0(module_fun);
    nlr_pop();
  } else {
    // uncaught exception
    mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
  }
}

void test_performance_micropython() {
  mp_init();

  do_str("import IC\n"
         "import uctypes\n"
         "S = {\"x\": 0 | uctypes.INT32, \"x1\": 4 | uctypes.INT32, \"x2\": 8 "
         "| uctypes.INT32, \"y1\": 12 | uctypes.INT32, \"y2\": 16| "
         "uctypes.INT32}\n"
         "s = uctypes.struct(IC.get_struct(), S, uctypes.NATIVE)\n",
         MP_PARSE_FILE_INPUT);

  do_str("def lerp():\n"
         "    return s.y1 + ((s.y2 - s.y1) * (s.x - s.x1)) // (s.x2 - s.x1)",
         MP_PARSE_FILE_INPUT);

  s.x = 5;
  s.x1 = 1;
  s.x2 = 10;
  s.y1 = 5;
  s.y2 = 50;

  do_str("retval = lerp()\n"
         "print(retval)",
         MP_PARSE_FILE_INPUT);

  mp_obj_t compiled_lerp = compile_str("retval = lerp()\n"
                                       "print(retval)",
                                       MP_PARSE_FILE_INPUT);

  call_bytecode(compiled_lerp);

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
