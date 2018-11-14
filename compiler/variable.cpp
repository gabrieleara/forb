
#include <variable.hpp>

#include "types/type.hpp"
#include "variable.hpp"

using code_ostream = forbcc::code_ostream;

using type = forbcc::type;
using variable = forbcc::variable;

void variable::print_declaration(code_ostream &output) const {
    this->var_type.print_var_declaration(output, name);
}

void variable::print_reference(code_ostream &output) const {
    this->var_type.print_var_reference(output, name);
}

void forbcc::variable::print_marshal(code_ostream &output) const {
    var_type.print_var_marshal(output, name);
}

void forbcc::variable::print_unmarshal(code_ostream &output) const {
    var_type.print_var_unmarshal(output, name);
}

void forbcc::variable::print_recv(code_ostream &output) const {
    var_type.print_var_recv(output, name);
}

void forbcc::variable::print_send(code_ostream &output) const {
    var_type.print_var_send(output, name);
}
