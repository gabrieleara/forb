//
// Created by gabriele on 14/11/18.
//

#include "module.hpp"
#include "code_ostream.hpp"

// For documentation, see corresponding header file

/// The global module has no parent module
const std::shared_ptr<forbcc::module> forbcc::module::global_module = forbcc::module::new_ptr(nullptr);

void forbcc::module::print_declaration(forbcc::code_ostream &out) const {
    if (name().length()) {
        out << "namespace " << name() << " {" << std::endl;
        out.increment_indentation();
    }

    for (const auto &it : list()) {
        it->print_declaration(out);
    }

    if (name().length()) {
        out.decrement_indentation();
        out << "} // namespace " << name() << std::endl;
    }

}

void forbcc::module::print_definition(forbcc::code_ostream &out) const {
    for (const auto &it : list()) {
        it->print_definition(out);
    }
}


