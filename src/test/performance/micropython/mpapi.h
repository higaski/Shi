#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "py/compile.h"
#include "py/mperrno.h"
#include "py/repl.h"
#include "py/runtime.h"

void mp_dostring(const char* src, mp_parse_input_kind_t input_kind);
mp_obj_t mp_compilestring(const char* src, mp_parse_input_kind_t input_kind);
void mp_dobytecode(mp_obj_t module_fun);

#ifdef __cplusplus
}
#endif
