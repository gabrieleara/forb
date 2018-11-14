
#include <types/array_type.hpp>

#include "code_ostream.hpp"
#include "types/array_type.hpp"
#include "types/type.hpp"
#include "types/custom_type.hpp"
#include "variable.hpp"

using type = forbcc::type;
using array_type = forbcc::array_type;

std::string array_type::get_codename() const {
    return item_type.get_codename();// + "[" + std::to_string(length) + "]";
}

std::string array_type::get_reference_name() const {
    return item_type.get_codename() + "*";
}

// TODO: check that probably this one prints in the opposite order!
void array_type::print_var_declaration(code_ostream &output, const std::string &var_name) const {
    output << item_type.get_codename() << " " << var_name << "[" << length << "]";

    const array_type *is_array = dynamic_cast<const array_type *>(&item_type);

    while (is_array != nullptr) {
        // The array is actually an array of arrays
        output << "[" << is_array->length << "]";
        is_array = dynamic_cast<const array_type *>(&is_array->item_type);
    }
}

void array_type::print_var_reference(code_ostream &output, const std::string &var_name) const {
    return print_var_declaration(output, var_name);
}

void forbcc::array_type::print_var_marshal(forbcc::code_ostream &output, const std::string &var_name) const {
    /*
    output << "for(int __i__ = 0; __i__ < " << length << "; ++__i__) {" << std::endl;
    output.increment_indentation();
    item_type.print_var_marshal(output, var_name + "[__i__]");
    output.decrement_indentation();
    output << "}" << std::endl;
     */

    output << "forb::streams::marshal(" << var_name << ", " << length << ");" << std::endl;
}

void forbcc::array_type::print_var_unmarshal(forbcc::code_ostream &output, const std::string &var_name) const {
    /*
    output << "for(int __i__ = 0; __i__ < " << length << "; ++__i__) {" << std::endl;
    output.increment_indentation();
    item_type.print_var_unmarshal(output, var_name + "[__i__]");
    output.decrement_indentation();
    output << "}" << std::endl;
     */

    output << "forb::streams::unmarshal(" << var_name << ", " << length << ");" << std::endl;
}

void forbcc::array_type::print_var_recv(forbcc::code_ostream &output, const std::string &var_name) const {
    output << "datastream->recv(" << var_name << ", sizeof(" << item_type.name << ") * " << length << ");" << std::endl;
}

void forbcc::array_type::print_var_send(forbcc::code_ostream &output, const std::string &var_name) const {
    output << "datastream->send(" << var_name << ", sizeof(" << item_type.name << ") * " << length << ");" << std::endl;
}
