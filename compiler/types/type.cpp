#include <string>

#include <code_ostream.hpp>
#include <types/type.hpp>

// For documentation, see corresponding header file

void forbcc::type::print_var_declaration(forbcc::code_ostream &out, const std::string &var_name) const {
    out << codename() << " " << var_name;
}

void forbcc::type::print_var_definition(forbcc::code_ostream &out, const std::string &var_name,
                                        bool force_stack) const {
    if (force_stack || size_of() <= forbcc::max_stack_size) {
        print_var_declaration(out, var_name);
    } else {
        out << "std::unique_ptr<" << codename() << "> " << var_name
            << " = std::make_unique<" << codename() << ">();";
    }
}

void forbcc::type::print_var_lvalue(forbcc::code_ostream &out, const std::string &var_name, bool force_stack) const {
    if (force_stack || size_of() <= forbcc::max_stack_size) {
        out << var_name;
    } else {
        out << "*" << var_name;
    }
}

void forbcc::type::print_var_reference(forbcc::code_ostream &out, const std::string &var_name) const {
    out << codename() << " &" << var_name;
}

void forbcc::type::print_var_marshal(forbcc::code_ostream &out, const std::string &var_name,
                                     const forbcc::marshal &do_undo, bool force_stack) const {

    std::string action = (do_undo == marshal::MARSHAL) ? "marshal" : "unmarshal";
    print_var_lvalue(out, var_name, force_stack);
    out << " = forb::streams::" << action << "(";
    print_var_lvalue(out, var_name, force_stack);
    out << ");" << std::endl;
}

void forbcc::type::print_var_serialize(forbcc::code_ostream &out, const std::string &var_name,
                                       const forbcc::serialize &do_undo, bool force_stack) const {

    std::string action = (do_undo == serialize::SEND) ? "send" : "recv";
    out << "datastream->" << action << "(&";
    print_var_lvalue(out, var_name, force_stack);
    out << ", sizeof(";
    print_var_lvalue(out, var_name, force_stack);
    out << "));" << std::endl;
}
