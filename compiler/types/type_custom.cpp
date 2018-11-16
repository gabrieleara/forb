#include <map> // std::map, pair

#include "../include/types/type_custom.hpp"
#include "../include/module.hpp"

#include "../include/code_ostream.hpp"
#include "../include/variable.hpp"

void forbcc::type_custom::print_declaration(code_ostream &out) const {
    out << "struct " << name << " {" << std::endl;

    out.increment_indentation();

    for (const auto &it : list()) {
        it.print_declaration(out);
        out << ";" << std::endl;
    }

    out.decrement_indentation();

    out << "};" << std::endl;

    out << std::endl;
}


void forbcc::type_custom::print_definition(forbcc::code_ostream &out) const {
    // Custom types define a partial specialization for the marshal template wihtin forb namespace
    out << "namespace forb {" << std::endl;
    out.increment_indentation();
    out << "namespace streams {" << std::endl;
    out.increment_indentation();

    out << "template <>" << std::endl
        << codename() << " marshal<" << codename() << ">(" << codename() << " v) {" << std::endl;

    out.increment_indentation();

    for (const auto & it : list()) {
        it.var_type.print_var_marshal(out, "v." + it.name, marshal::MARSHAL);
    }

    out << "return v;" << std::endl;

    out.decrement_indentation();
    out << "}" << std::endl;

    out.decrement_indentation();
    out << "} // namespace streams" << std::endl;

    out.decrement_indentation();
    out << "} // namespace forb" << std::endl
        << std::endl;
}

/*
void forbcc::type_custom::print_var_marshal(forbcc::code_ostream &out, const std::string &var_name) const {
    for (const auto &it : attributes) {
        const variable &attr = it.second;

        attr.var_type.print_var_marshal(out, var_name + "." + attr.name);
    }
}

void forbcc::type_custom::print_var_unmarshal(forbcc::code_ostream &out, const std::string &var_name) const {
    for (const auto &it : attributes) {
        const variable &attr = it.second;

        attr.var_type.print_var_unmarshal(out, var_name + "." + attr.name);
    }
}
 */