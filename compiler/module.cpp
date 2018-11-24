//
// Created by gabriele on 14/11/18.
//

#include <module.hpp>

#include "module.hpp"
#include "code_ostream.hpp"

#include "types/type_custom.hpp"


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
        out << std::endl;
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

std::shared_ptr<forbcc::module> forbcc::module::find_module(const std::string &name) {
    return std::dynamic_pointer_cast<module>(find(name));
}

std::shared_ptr<forbcc::type_custom> forbcc::module::find_type(const std::string &name) {
    return std::dynamic_pointer_cast<type_custom>(find(name));
}

std::shared_ptr<forbcc::entity> forbcc::module::find(const std::string &name) {
    std::string::size_type index = name.find_last_of(":");

    if (index != std::string::npos) {
        // The name is fully qualified, redo find with less qualified name
        module::ptr_t m = find_module(name.substr(0, index-1));

        // Then perform the find on the found module
        return m->find(name.substr(index+1));
    }

    if (!contains(name)) {
        // Cannot find requested type in this scope.
        // Look also in parent module, since current scope comprehends also parent's scope.
        module::ptr_t my_parent = std::static_pointer_cast<module>(parent());

        // This is false only for global_module of course
        if (my_parent != nullptr) {
            return my_parent->find(name);
        } else {
            // Could not find anywhere the requested type
            return nullptr;
        }
    } else {
        return (*this)[name];
    }
}



