#include <string>

#include <code_ostream.hpp>
#include <types/type.hpp>

// For documentation, see corresponding header file

void forbcc::type::print_var_declaration(forbcc::code_ostream &out, const std::string &var_name) const {
    out << codename() << " " << var_name;
}

void forbcc::type::print_var_reference(forbcc::code_ostream &out, const std::string &var_name) const {
    out << codename() << " &" << var_name;
}

void forbcc::type::print_var_marshal(forbcc::code_ostream &out, const std::string &var_name,
                                     const forbcc::marshal &do_undo) const {

    std::string action = (do_undo == marshal::MARSHAL) ? "marshal" : "unmarshal";
    out << var_name << " = forb::streams::" << action << "(" << var_name << ");" << std::endl;
}

void forbcc::type::print_var_serialize(forbcc::code_ostream &out, const std::string &var_name,
                                       const forbcc::serialize &do_undo) const {

    std::string action = (do_undo == serialize::SEND) ? "send" : "recv";
    out << "datastream->" << action << "(&" << var_name << ", sizeof(" << var_name << "));" << std::endl;
}
