#include "mpapi.h"

typedef struct {
  int32_t x, x1, x2, y1, y2;
} S;

S s = {};

STATIC mp_obj_t lerp_get_struct(void) {
  return mp_obj_new_int((int)(&s));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(lerp_get_struct_obj, lerp_get_struct);

STATIC const mp_map_elem_t lerp_globals_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_lerp_module)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_get_struct), (mp_obj_t)&lerp_get_struct_obj},
};

STATIC MP_DEFINE_CONST_DICT(mp_module_lerp_globals, lerp_globals_table);

const mp_obj_module_t mp_module_lerp = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t*)&mp_module_lerp_globals,
};

void performance_mp_lerp() {
  mp_init();

  mp_dostring(
      "import lerp_module\n"
      "import uctypes\n"
      "S = {\"x\": 0 | uctypes.INT32, \"x1\": 4 | uctypes.INT32, \"x2\": 8 "
      "| uctypes.INT32, \"y1\": 12 | uctypes.INT32, \"y2\": 16| "
      "uctypes.INT32}\n"
      "s = uctypes.struct(lerp.get_struct(), S, uctypes.NATIVE)\n",
      MP_PARSE_FILE_INPUT);

  mp_dostring(
      "def lerp():\n"
      "    return s.y1 + ((s.y2 - s.y1) * (s.x - s.x1)) // (s.x2 - s.x1)",
      MP_PARSE_FILE_INPUT);

  s.x = 5;
  s.x1 = 1;
  s.x2 = 10;
  s.y1 = 5;
  s.y2 = 50;

  mp_dostring("retval = lerp()\n"
              "print(retval)",
              MP_PARSE_FILE_INPUT);

  mp_obj_t compiled_lerp = mp_compilestring("retval = lerp()\n"
                                            "print(retval)",
                                            MP_PARSE_FILE_INPUT);

  mp_dobytecode(compiled_lerp);

  mp_deinit();
}
