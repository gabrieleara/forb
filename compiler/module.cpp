//
// Created by gabriele on 14/11/18.
//

#include "module.hpp"
#include "code_ostream.hpp"

#include "types/type_struct.hpp"


// For documentation, see corresponding header file

/// The global module has no parent module of course.
const std::shared_ptr<forbcc::module> forbcc::module::global_module = forbcc::module::new_ptr();

void forbcc::module::print_declaration(forbcc::code_ostream &out) const {
    if (name().length()) {
        out << "namespace " << name() << " {" << std::endl;
        out.increment_indentation();
    }

    // Print the declaration of each entity in the given namespace
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
    // Simply print the definition of each entity in the namespace.
    // Definitions will fully-qualify each name automatically, so no special behavior of the module is required.
    for (const auto &it : list()) {
        it->print_definition(out);
    }
}

std::shared_ptr<forbcc::module> forbcc::module::get_module(const std::string &name) {
    return std::dynamic_pointer_cast<forbcc::module>((*this)[name]);
}

std::shared_ptr<forbcc::module> forbcc::module::find_module(const std::string &name) {
    return std::dynamic_pointer_cast<module>(find(name));
}

std::shared_ptr<forbcc::type_struct> forbcc::module::find_struct(const std::string &name) {
    return std::dynamic_pointer_cast<type_struct>(find(name));
}

std::shared_ptr<forbcc::entity> forbcc::module::find(const std::string &name) {
    // FIXME: Potentially, a qualified structure/namespace such as b::c could be found even if it has been defined in a parent of its namespace (for example a::c, where b is in fact a::b). This situation should lead to an error, but right now it doesn't.
    std::string::size_type index = name.find_last_of(":");

    if (index != std::string::npos) {
        // The name is fully qualified, redo find with less qualified name
        module::ptr_t m = find_module(name.substr(0, index - 1));

        // Then perform the find on the found module
        return m->find(name.substr(index + 1));
    }

    if (!contains(name)) {
        // Cannot find requested entity in this scope.
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



