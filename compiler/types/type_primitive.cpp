//
// Created by gabriele on 18/11/18.
//
#include "types/type_primitive.hpp"

forbcc::type_primitive::type_primitive_list forbcc::type_primitive::known_types;

static const forbcc::type_primitive::ptr_const_t _Void  = forbcc::type_primitive::new_ptr_const("void", "void");
static const forbcc::type_primitive::ptr_const_t _Char  = forbcc::type_primitive::new_ptr_const("char", "char");
static const forbcc::type_primitive::ptr_const_t _Int   = forbcc::type_primitive::new_ptr_const("int", "int");
static const forbcc::type_primitive::ptr_const_t _Short = forbcc::type_primitive::new_ptr_const("short", "short");
static const forbcc::type_primitive::ptr_const_t _Long  = forbcc::type_primitive::new_ptr_const("long", "long");

forbcc::type_primitive::static_initializer::static_initializer() {
    type_primitive::known_types.insert("void", _Void);
    type_primitive::known_types.insert("char", _Char);
    type_primitive::known_types.insert("int", _Int);
    type_primitive::known_types.insert("short", _Short);
    type_primitive::known_types.insert("long", _Long);
}

forbcc::type_primitive::static_initializer forbcc::type_primitive::_init;

