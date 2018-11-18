
#include "types/type_array.hpp"

#include "code_ostream.hpp"
#include "types/type_array.hpp"
#include "types/type.hpp"
#include "types/type_custom.hpp"
#include "variable.hpp"

// For documentation, see corresponding header file

void forbcc::type_array::print_var_declaration(forbcc::code_ostream &output, const std::string &var_name) const {
    output << _item_type->codename() << " " << var_name << "[" << _length << "]";
}

void forbcc::type_array::print_var_reference(forbcc::code_ostream &output, const std::string &var_name) const {
    return print_var_declaration(output, var_name);
}

void forbcc::type_array::print_var_marshal(forbcc::code_ostream &out, const std::string &var_name,
                                           const forbcc::marshal &do_undo) const {

    std::string action = (do_undo == marshal::MARSHAL) ? "marshal" : "unmarshal";
    out << "forb::streams::" << action << "(" << var_name << ", " << _length << ");" << std::endl;
}

void forbcc::type_array::print_var_serialize(forbcc::code_ostream &out, const std::string &var_name,
                                             const forbcc::serialize &do_undo) const {

    std::string action = (do_undo == serialize::SEND) ? "send" : "recv";
    out << "datastream->" << action << "(" << var_name << ", sizeof(" << _item_type->name() << ") * " << length()
        << ");"
        << std::endl;
}
