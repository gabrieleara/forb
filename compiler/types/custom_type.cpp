#include <map> // std::map, pair

#include "types/custom_type.hpp"
#include "types/module.hpp"

#include "code_ostream.hpp"
#include "variable.hpp"

template<typename K, typename V>
inline bool insert_map_simplified(std::map<K, V> &mymap, K key, V value) {
    return mymap.insert(std::make_pair(key, value)).second;
}

using custom_type = forbcc::custom_type;

std::string custom_type::get_codename() const {
    return themodule.get_codename() + "::" + type::get_codename();
}

bool custom_type::insert_attribute(const variable &attr) {
    return insert_map_simplified(attributes, attr.name, attr);
}

void custom_type::print_declaration(code_ostream &output) const {
    output << "struct " << name << " {" << std::endl;
    // output << "public:" << std::endl;

    output.increment_indentation();

    for (const auto &it : attributes) {
        it.second.print_declaration(output);
        output << ";";
        output << std::endl;
    }

    output.decrement_indentation();

    output << "};" << std::endl;

    output << std::endl;

/*
    output.increment_indentation();

    for (const auto &it : attributes) {
        it.second.print_getter_declaration(output);
        output << std::endl;
    }

    output << std::endl;

    for (const auto &it : attributes) {
        it.second.print_setter_declaration(output);
        output << std::endl;
    }

    // TODO: print other stuff

    output.decrement_indentation();
*/
}

// TODO: decide whether I need the definition of these types, since they are simply structs
void custom_type::print_definition(code_ostream &output __attribute__((unused)),
                                   const std::string &scope __attribute__((unused))) const {
    // output << "using " << name << " = " << scope << "::" << name << ";"
    //        << std::endl << std::endl;

/*
    for (auto it : attributes) {
        it.second.print_getter_definition(output);
        output << std::endl;
    }

    for (auto it : attributes) {
        it.second.print_setter_definition(output);
        output << std::endl;
    }
*/

    // TODO: print other stuff
}

void forbcc::custom_type::print_var_marshal(forbcc::code_ostream &output, const std::string &var_name) const {
    for (const auto &it : attributes) {
        const variable &attr = it.second;

        attr.var_type.print_var_marshal(output, var_name + "." + attr.name);
    }
}

void forbcc::custom_type::print_var_unmarshal(forbcc::code_ostream &output, const std::string &var_name) const {
    for (const auto &it : attributes) {
        const variable &attr = it.second;

        attr.var_type.print_var_unmarshal(output, var_name + "." + attr.name);
    }
}
