#include <string>

#include "code_ostream.hpp"
#include "types/type.hpp"
#include "types/custom_type.hpp"
#include "variable.hpp"


using code_ostream = forbcc::code_ostream;
using type = forbcc::type;
using variable = forbcc::variable;

type::~type() {}

std::string type::get_codename() const {
    return name;
}

std::string type::get_reference_name() const {
    return get_codename() + "&&";
}

void type::print_declaration(code_ostream &output __attribute__((unused))) const {
    // By default, no declaration is needed, only custom types require
    // declaration
}

void type::print_definition(code_ostream &output __attribute__((unused)),
                            const std::string &scope __attribute__((unused))) const {
    // Same as declaration
}

void type::print_var_declaration(code_ostream &output, const std::string &var_name) const {
    output << get_codename() << " " << var_name;
}

void type::print_var_reference(code_ostream &output, const std::string &var_name) const {
    output << get_codename() << " &" << var_name;
}

void type::print_var_marshal(code_ostream &output, const std::string &var_name) const {
    output << var_name << " = forb::streams::marshal(" << var_name << ");" << std::endl;
}

void type::print_var_unmarshal(code_ostream &output, const std::string &var_name) const {
    output << var_name << " = forb::streams::unmarshal(" << var_name << ");" << std::endl;
}

void type::print_var_recv(code_ostream &output, const std::string &var_name) const {
    output << "datastream->recv(&" << var_name << ", sizeof(" << var_name << "));" << std::endl;
}

void type::print_var_send(code_ostream &output, const std::string &var_name) const {
    output << "datastream->send(&" << var_name << ", sizeof(" << var_name << "));" << std::endl;
}

