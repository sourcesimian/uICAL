#include <stdio.h>

#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/binary.h"

#include "rrule.h"

typedef struct _rrule_obj_t {
    mp_obj_base_t base;
    uint8_t hello_number;
    rrule_p this;
} rrule_obj_t;


STATIC mp_obj_t rrule_next(mp_obj_t self_in) {
    rrule_obj_t *self = MP_OBJ_TO_PTR(self_in);
    RRule_next(self->this);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(rrule_increment_obj, rrule_increment);


STATIC mp_obj_t rrule_increment(mp_obj_t self_in) {
    rrule_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->hello_number += 1;
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(rrule_increment_obj, rrule_increment);

STATIC mp_obj_t rrule_free(mp_obj_t self_in) {
    rrule_obj_t *self = MP_OBJ_TO_PTR(self_in);
    RRule_del(self->this);
    m_del_obj(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(rrule_free_obj, rrule_free);


const mp_obj_type_t rrule_type;

mp_obj_t rrule_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // this checks the number of arguments (min 1, max 1);
    // on error -> raise python exception
    mp_arg_check_num(n_args, n_kw, 1, 1, true);
    // create a new object of our C-struct type
    rrule_obj_t *self = m_new_obj(rrule_obj_t);
    // give it a type
    self->base.type = &rrule_type;
    // set the member number with the first argument of the constructor
    self->hello_number = mp_obj_get_int(args[0]);
    self->this = RRule_new("FREQ=MONTHLY;BYDAY=FR;BYMONTHDAY=13", "19700101T090000");
    return MP_OBJ_FROM_PTR(self);
}

STATIC void rrule_print( const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    // get a ptr to the C-struct of the object
    rrule_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // print the number
    printf ("Hello(%u)", self->hello_number);
}


// creating the table of global members
STATIC const mp_rom_map_elem_t rrule_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_inc), MP_ROM_PTR(&rrule_increment_obj) },
    { MP_ROM_QSTR(MP_QSTR_free), MP_ROM_PTR(&rrule_free_obj) },
};
STATIC MP_DEFINE_CONST_DICT(rrule_locals_dict, rrule_locals_dict_table);

// create the class-object itself
const mp_obj_type_t rrule_type = {
    { &mp_type_type },
    .name = MP_QSTR_RRUle,
    .print = rrule_print,
    .make_new = rrule_make_new,
    .locals_dict = (mp_obj_dict_t*)&rrule_locals_dict,
};