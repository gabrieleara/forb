#include <types/type_struct.hpp>

#include <code_ostream.hpp>
#include <variable.hpp>

// For documentation, see corresponding header file

void forbcc::type_struct::print_declaration(code_ostream &out) const {
    out << "struct " << name() << " {" << std::endl;

    out.increment_indentation();

    // Print each attribute declaration
    for (const auto &it : list()) {
        it.print_declaration(out);
        out << ";" << std::endl;
    }

    out.decrement_indentation();

    out << "};" << std::endl;

    out << std::endl;
}


void forbcc::type_struct::print_definition(forbcc::code_ostream &out) const {
    // Structure types define a partial specialization for the marshal template wihtin forb namespace, so any variable
    // can be simply marshalled/unmarshalled by calling forbcc::marshal or forbcc::unmarshal.
    out << "namespace forb {" << std::endl;
    out.increment_indentation();
    out << "namespace streams {" << std::endl;
    out.increment_indentation();

    out << "template <>" << std::endl
        << codename() << " marshal<" << codename() << ">(" << codename() << " v) {" << std::endl;

    out.increment_indentation();

    for (const auto &it : list()) {
        it.var_type()->print_var_marshal(out, "v." + it.name(), marshal::MARSHAL);
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