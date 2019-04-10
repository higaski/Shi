#include "bench.h"
#include "mpapi.h"

static int retval = 0;

STATIC mp_obj_t acker_result(mp_obj_t n) {
  retval = mp_obj_get_int(n);
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(acker_obj, acker_result);

STATIC const mp_map_elem_t acker_globals_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_acker_module)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_acker_result), (mp_obj_t)&acker_obj},
};

STATIC MP_DEFINE_CONST_DICT(mp_module_acker_globals, acker_globals_table);

const mp_obj_module_t mp_acker_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t*)&mp_module_acker_globals,
};

int mp_acker() {
  mp_dostring("import acker_module\n"
              "def ack(M, N):\n"
              "    return (N + 1) if M == 0 else (\n"
              "        ack(M-1, 1) if N == 0 else ack(M-1, ack(M, N-1)))",
              MP_PARSE_FILE_INPUT);

  mp_obj_t ack_bytecode = mp_compilestring("n = ack(3,2)", MP_PARSE_FILE_INPUT);

  START_CYC_CNT();
  mp_dobytecode(ack_bytecode);
  STOP_CYC_CNT();

  mp_dostring("acker_module.acker_result(n)", MP_PARSE_FILE_INPUT);

  return retval == 29 ? 0 : 1;
}
