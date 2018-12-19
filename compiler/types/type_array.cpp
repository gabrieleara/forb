
#include <types/type_array.hpp>

#include <code_ostream.hpp>
#include <variable.hpp>

#include <types/type_array.hpp>
#include <types/type.hpp>
#include <types/type_struct.hpp>

// For documentation, see corresponding header file

forbcc::type_array::type_array_list forbcc::type_array::arrays;

void forbcc::type_array::print_var_declaration(forbcc::code_ostream &out, const std::string &var_name) const {
    out << _item_type->codename() << " " << var_name << "[" << _length << "]";
}

void forbcc::type_array::print_var_definition(forbcc::code_ostream &out, const std::string &var_name,
                                              bool force_stack) const {

    if (force_stack || size_of() <= forbcc::max_stack_size) {
        print_var_declaration(out, var_name);
    } else {
        out << "std::unique_ptr<" << _item_type->codename() << "[]> " << var_name
            << " = std::make_unique<" << _item_type->codename() << "[]>"
            << "(" << _length << ")";
    }
}

void forbcc::type_array::print_var_lvalue(forbcc::code_ostream &out, const std::string &var_name,
                                          bool force_stack) const {
    out << var_name;

    if (force_stack || size_of() <= forbcc::max_stack_size) {
        return;
    }

    out << ".get()";
}

void forbcc::type_array::print_var_reference(forbcc::code_ostream &out, const std::string &var_name) const {
    return print_var_declaration(out, var_name);
}

void forbcc::type_array::print_var_marshal(forbcc::code_ostream &out, const std::string &var_name,
                                           const forbcc::marshal &do_undo, bool force_stack) const {

    // Arrays use an overload of forbcc::marshal template that accepts the length of the array as argument
    std::string action = (do_undo == marshal::MARSHAL) ? "marshal" : "unmarshal";
    out << "forb::streams::" << action << "(";

    print_var_lvalue(out, var_name, force_stack);

    out << ", " << _length << ");" << std::endl;
}

void forbcc::type_array::print_var_serialize(forbcc::code_ostream &out, const std::string &var_name,
                                             const forbcc::serialize &do_undo, bool force_stack) const {

    std::string action = (do_undo == serialize::SEND) ? "send" : "recv";
    out << "datastream->" << action << "(";

    print_var_lvalue(out, var_name, force_stack);

    out << ", sizeof(" << _item_type->codename() << ") * " << length()
        << ");"
        << std::endl;
}
