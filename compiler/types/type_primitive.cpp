//
// Created by gabriele on 18/11/18.
//
#include <types/type_primitive.hpp>

// For documentation, see corresponding header file

forbcc::type_primitive::type_primitive_list forbcc::type_primitive::known_types;

static const forbcc::type_primitive::ptr_const_t _Void  = forbcc::type_primitive::new_ptr_const("void", "void", 0);
static const forbcc::type_primitive::ptr_const_t _Char  = forbcc::type_primitive::new_ptr_const("char", "int8_t",
                                                                                                sizeof(int8_t));
static const forbcc::type_primitive::ptr_const_t _Int   = forbcc::type_primitive::new_ptr_const("int", "int32_t",
                                                                                                sizeof(int32_t));
static const forbcc::type_primitive::ptr_const_t _Short = forbcc::type_primitive::new_ptr_const("short", "int16_t",
                                                                                                sizeof(int16_t));
static const forbcc::type_primitive::ptr_const_t _Long  = forbcc::type_primitive::new_ptr_const("long", "int32_t",
                                                                                                sizeof(int32_t));

forbcc::type_primitive::static_initializer::static_initializer() {
    type_primitive::known_types.insert("void", _Void);
    type_primitive::known_types.insert("char", _Char);
    type_primitive::known_types.insert("int", _Int);
    type_primitive::known_types.insert("short", _Short);
    type_primitive::known_types.insert("long", _Long);
}

forbcc::type_primitive::static_initializer forbcc::type_primitive::_init;

