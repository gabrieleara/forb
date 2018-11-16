
#include "../include/types/type_array.hpp"

#include "../include/code_ostream.hpp"
#include "../include/types/type_array.hpp"
#include "../include/types/type.hpp"
#include "../include/types/type_custom.hpp"
#include "../include/variable.hpp"

using type = forbcc::type;
using array_type = forbcc::type_array;

void array_type::print_var_declaration(code_ostream &output, const std::string &var_name) const {
    output << item_type.codename() << " " << var_name << "[" << length << "]";

    /*
    // _TODO: check that probably this one prints in the opposite order!
    const type_array *is_array = dynamic_cast<const type_array *>(&item_type);

    while (is_array != nullptr) {
        // The array is actually an array of arrays
        output << "[" << is_array->length << "]";
        is_array = dynamic_cast<const type_array *>(&is_array->item_type);
    }
     */
}

void array_type::print_var_reference(code_ostream &output, const std::string &var_name) const {
    return print_var_declaration(output, var_name);
}

void forbcc::type_array::print_var_marshal(forbcc::code_ostream &out, const std::string &var_name,
                                           const forbcc::marshal do_undo) const {
    std::string action = (do_undo == marshal::MARSHAL) ? "marshal" : "unmarshal";
    out << "forb::streams::" << action << "(" << var_name << ", " << length << ");" << std::endl;
}

void forbcc::type_array::print_var_serialize(forbcc::code_ostream &out, const std::string &var_name,
                                             const forbcc::serialize do_undo) const {

    std::string action = (do_undo == serialize::SEND) ? "send" : "recv";
    out << "datastream->" << action << "(" << var_name << ", sizeof(" << item_type.name << ") * " << length << ");"
        << std::endl;
}

/*
void forbcc::type_array::print_var_marshal(forbcc::code_ostream &output, const std::string &var_name) const {
    output << "for(int __i__ = 0; __i__ < " << length << "; ++__i__) {" << std::endl;
    output.increment_indentation();
    item_type.print_var_marshal(output, var_name + "[__i__]");
    output.decrement_indentation();
    output << "}" << std::endl;

    // output << "forb::streams::marshal(" << var_name << ", " << length << ");" << std::endl;
}

void forbcc::type_array::print_var_unmarshal(forbcc::code_ostream &output, const std::string &var_name) const {
    output << "for(int __i__ = 0; __i__ < " << length << "; ++__i__) {" << std::endl;
    output.increment_indentation();
    item_type.print_var_unmarshal(output, var_name + "[__i__]");
    output.decrement_indentation();
    output << "}" << std::endl;

    // output << "forb::streams::unmarshal(" << var_name << ", " << length << ");" << std::endl;
}
*/
