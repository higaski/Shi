#include "bench.h"
#include "lerp_data.h"
#include "mpapi.h"

typedef struct {
  int32_t x, x1, x2, y1, y2, y;
} Lerp;

static Lerp lerp = {};

STATIC mp_obj_t lerp_get_struct(void) {
  return mp_obj_new_int((int)(&lerp));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(lerp_get_struct_obj, lerp_get_struct);

STATIC const mp_map_elem_t lerp_globals_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_lerp_module)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_get_struct), (mp_obj_t)&lerp_get_struct_obj},
};

STATIC MP_DEFINE_CONST_DICT(mp_module_lerp_globals, lerp_globals_table);

const mp_obj_module_t mp_lerp_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t*)&mp_module_lerp_globals,
};

int mp_lerp() {
  mp_dostring(
      "import lerp_module\n"
      "import uctypes\n"
      "S = {\"x\": 0 | uctypes.INT32,"
      "\"x1\": 4 | uctypes.INT32,"
      "\"x2\": 8 | uctypes.INT32,"
      "\"y1\": 12 | uctypes.INT32,"
      "\"y2\": 16| uctypes.INT32,"
      "\"y\": 20 | uctypes.INT32}\n"
      "s = uctypes.struct(lerp_module.get_struct(), S, uctypes.NATIVE)\n",
      MP_PARSE_FILE_INPUT);

  mp_dostring(
      "def lerp():\n"
      "    s.y = s.y1 + ((s.y2 - s.y1) * (s.x - s.x1)) // (s.x2 - s.x1)",
      MP_PARSE_FILE_INPUT);

  mp_obj_t lerp_bytecode = mp_compilestring("lerp()", MP_PARSE_FILE_INPUT);

  size_t const n = sizeof(lerp_data.x) / sizeof(lerp_data.x[0]);

  int32_t* y = (int32_t*)malloc(sizeof(lerp_data.x));

  START_CYC_CNT();
  for (size_t i = 0; i < n; ++i) {
    lerp.x = lerp_data.x[i];
    lerp.x1 = lerp_data.x1[i];
    lerp.x2 = lerp_data.x2[i];
    lerp.y1 = lerp_data.y1[i];
    lerp.y2 = lerp_data.y2[i];
    mp_dobytecode(lerp_bytecode);
    y[i] = lerp.y;
  }
  STOP_CYC_CNT();

  bool equal = true;
  for (size_t i = 0; i < n; ++i) {
    if (y[i] != lerp_data.y[i])
      equal = false;
  }

  free(y);

  return equal ? 0 : 1;
}
