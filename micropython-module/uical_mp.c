#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"


// This is the function which will be called from Python as example.add_ints(a, b).
STATIC mp_obj_t uical_add_ints(mp_obj_t a_obj, mp_obj_t b_obj) {
    // Extract the ints from the micropython input objects
    int a = mp_obj_get_int(a_obj);
    int b = mp_obj_get_int(b_obj);

    // Calculate the addition and convert to MicroPython object.
    return mp_obj_new_int(a + b);
}
// Define a Python reference to the function above
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uical_add_ints_obj, uical_add_ints);

// Define all properties of the module.
// Table entries are key/value pairs of the attribute name (a string)
// and the MicroPython object reference.
extern const mp_obj_type_t rrule_type;

STATIC const mp_rom_map_elem_t uical_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_uICAL) },
    { MP_ROM_QSTR(MP_QSTR_add_ints), MP_ROM_PTR(&uical_add_ints_obj) },
    { MP_ROM_QSTR(MP_QSTR_RRule), MP_ROM_PTR(&rrule_type) },

};
STATIC MP_DEFINE_CONST_DICT(uical_module_globals, uical_module_globals_table);

// Define module object.
const mp_obj_module_t uical_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&uical_module_globals,
};

// Register the module to make it available in Python
MP_REGISTER_MODULE(MP_QSTR_uICAL, uical_user_cmodule, MODULE_UICAL_ENABLED);
