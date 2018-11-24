//
// Created by gabriele on 19/11/18.
//

#include "parser.hpp"
#include "preprocessor.hpp"

#include <type_traits>
#include <string>
#include <fstream>

#include <regex>

#include "module.hpp"
#include "types/type.hpp"
#include "types/type_array.hpp"
#include "types/type_custom.hpp"
#include "types/type_primitive.hpp"
#include "interface.hpp"
#include "method.hpp"

using namespace forbcc;

const std::string forbcc::keywords[] = {"module", "type", "interface", "in", "out", "inout"};

using keywords_under_t = typename std::underlying_type<keywords_idx_t>::type;

keywords_under_t _i(keywords_idx_t idx) {
    return static_cast<keywords_under_t>(idx);
}

inline void next(std::ifstream &file, std::string &str) {
    if (!(file >> str)) {
        // TODO: Unexpected EOF?
        throw 15;
    }
}

inline void skip_symbol(std::ifstream &file, const char symbol) {
    std::string str;

    next(file, str);

    if (str.length() != 1 && str[0] != symbol)
        throw 17;
}

inline bool is_valid(const std::string &name) {
    return std::regex_match(name, std::regex("([a-zA-Z_][a-zA-Z0-9_]*)"));
}

inline module::ptr_t module_declaration(std::ifstream &file, const module::ptr_t &parent) {
    module::ptr_t ptr;
    std::string   name;

    next(file, name);

    skip_symbol(file, '{');

    if (!is_valid(name)) {
        throw 37;
    }

    if (parent->contains(name)) {
        ptr = std::dynamic_pointer_cast<module>(parent->operator[](name));

        if (ptr == nullptr) {
            // The symbol is already defined and it does not refer to a module!
            throw 23;
        }
    } else {
        // A new entity must be created
        ptr = module::new_ptr(parent, name);
        parent->insert(name, ptr);
    }

    return ptr;
}

inline type_custom::ptr_t type_declaration(std::ifstream &file, const module::ptr_t &parent) {
    type_custom::ptr_t ptr;
    std::string        name;

    next(file, name);

    skip_symbol(file, '{');

    if (!is_valid(name)) {
        throw 37;
    }

    if (parent->contains(name)) {
        // The symbol is already defined within the same module!
        throw 23;
    }

    // A new entity must be created
    ptr = type_custom::new_ptr(parent, name);
    parent->insert(name, ptr);

    return ptr;
}

inline interface::ptr_t interface_declaration(std::ifstream &file, const module::ptr_t &parent) {
    interface::ptr_t ptr;
    std::string      name;

    next(file, name);

    skip_symbol(file, '{');

    if (!is_valid(name)) {
        throw 37;
    }

    if (parent->contains(name)) {
        // The symbol is already defined within the same module!
        throw 23;
    }
    // A new entity must be created
    ptr = interface::new_ptr(parent, name);
    parent->insert(name, ptr);

    return ptr;
}

std::shared_ptr<const type> find_type(const module::ptr_t &current_module, const std::string &type_str) {
    std::shared_ptr<const type> var_type = nullptr;

    if (type_primitive::known_types.contains(type_str)) {
        // It is a primitive type
        var_type = type_primitive::known_types[type_str];
    } else {
        // It is a custom type, which shall be either defined within this module or in another module
        // visible from this module
        var_type = current_module->find_type(type_str);
    }

    // If at this point var_tupe equals nullptr, the type is unknown
    if (var_type == nullptr) {
        throw 57;
    }

    return var_type;
}

/// Delimiter can be either ';' or ',' or ')', you can even give multiple characters in a list
variable variable_declaration(std::ifstream &file,
                              const module::ptr_t &current_module,
                              const std::string &type_str,
                              const std::string &delimiters,
                              std::string::size_type &delimiter_index) {

    std::shared_ptr<const type> var_type = find_type(current_module, type_str);

    // Now I need to get the variable's name
    std::string var_name;
    next(file, var_name);

    // Then I need to get a symbol, either the delimiter or [ for an array

    std::string input_str;
    next(file, input_str);

    if (input_str == "[") {
        // It's actually an array!
        long                        length;
        std::shared_ptr<const type> item_type = var_type;

        next(file, input_str);

        length = stol(input_str);

        if (length < 1) {
            throw 19;
        }

        skip_symbol(file, ']');

        std::string array_id = type_array::to_identifier(item_type, length);

        if (type_array::arrays.contains(array_id)) {
            var_type = type_array::arrays.operator[](array_id);
        } else {
            // I have to create a new array type
            type_array::ptr_t new_array = type_array::new_ptr(item_type, length);
            type_array::arrays.insert(array_id, new_array);
            var_type = new_array;
        }

        next(file, input_str);
    }

    delimiter_index = delimiters.find(input_str[0]);

    if (input_str.length() != 1 && delimiter_index >= delimiters.length()) {
        // Unexpected symbol
        throw 45;
    }

    return variable{var_type, var_name};
}

variable variable_declaration(std::ifstream &file,
                              const module::ptr_t &current_module,
                              const std::string &type_str,
                              const std::string &delimiters) {
    std::string::size_type dummy_index;
    return variable_declaration(file, current_module, type_str, delimiters, dummy_index);
}

parameter parameter_declaration(std::ifstream &file,
                                const module::ptr_t &current_module,
                                std::string &input_str,
                                bool &keep_going) {
    direction d;

    if (input_str == "in") {
        d = direction::IN;
    } else if (input_str == "out") {
        d = direction::OUT;
    } else if (input_str == "inout") {
        d = direction::INOUT;
    } else {
        throw 18;
    }

    next(file, input_str);

    std::string::size_type index;

    variable v = variable_declaration(file, current_module, input_str, ",)", index);

    keep_going = index != 1;

    return parameter{d, v};
}

method method_declaration(std::ifstream &file,
                          module::ptr_t &current_module,
                          interface::ptr_t &current_interf,
                          std::string input_str) {

    std::shared_ptr<const type> return_type = find_type(current_module, input_str);

    std::string method_name;

    next(file, method_name);

    method current_method{current_interf, method_name, return_type};

    skip_symbol(file, '(');

    bool keep_going = true;

    while (keep_going) {
        next(file, input_str);

        // Expecting either a closed ) or a parameter declaration
        if (input_str == ")") {
            keep_going = false;
            continue;
        }

        // Expecting now a parameter declaration
        parameter p = parameter_declaration(file, current_module, input_str, keep_going);

        if (!current_method.insert(p.var().name(), p)) {
            throw 15;
        }
    }

    skip_symbol(file, ';');

    return current_method;
}

void parse_within_type(std::ifstream &file, module::ptr_t &current_module, type_custom::ptr_t &current_type) {
    std::string input_str;

    bool keep_going = true;

    while (keep_going) {
        next(file, input_str);

        // Expecting either a closed } or a variable declaration
        if (input_str == "}") {
            keep_going = false;
            continue;
        }

        // Expecting now a variable declaration
        variable v = variable_declaration(file, current_module, input_str, ";");

        if (!current_type->insert(v.name(), v)) {
            throw 15;
        }
    }
}

void parse_within_interface(std::ifstream &file, module::ptr_t current_module, interface::ptr_t current_interf) {
    std::string input_str;

    bool keep_going = true;

    while (keep_going) {
        next(file, input_str);

        // Expecting either a closed } or a variable declaration
        if (input_str == "}") {
            keep_going = false;
            continue;
        }

        // Expecting now a variable declaration
        method m = method_declaration(file, current_module, current_interf, input_str);

        if (!current_interf->insert(m.id(), m)) {
            throw 19;
        }
    }
}


void parse_within_module(std::ifstream &file, module::ptr_t current_module) {
    std::string input_str;

    bool keep_going = true;

    while (keep_going && file >> input_str) {
        // Expecting either a "module", "type" or "interface" keyword
        if (input_str == forbcc::keywords[_i(keywords_idx_t::MODULE)]) {
            module::ptr_t new_module = module_declaration(file, current_module);

            parse_within_module(file, new_module);
        } else if (input_str == forbcc::keywords[_i(keywords_idx_t::TYPE)]) {
            type_custom::ptr_t new_type = type_declaration(file, current_module);

            parse_within_type(file, current_module, new_type);
        } else if (input_str == keywords[_i(keywords_idx_t::INTERFACE)]) {
            interface::ptr_t new_interface = interface_declaration(file, current_module);

            parse_within_interface(file, current_module, new_interface);
        } else if (input_str == "}") {
            if (current_module == module::global_module)
                throw 17;

            keep_going = false;
        } else {
            throw 42;
        }
    }

    if (!file && current_module != module::global_module) {
        // Unexpected end of file
        throw 73;
    }
}

void forbcc::parser::execute() {
    std::ifstream file{filename};

    module::ptr_t new_module = module::global_module;

    parse_within_module(file, new_module);
}


#include <iostream>
#include "code_ostream.hpp"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Please give as argument the name of the *.forb file to be processed." << std::endl;
        return EXIT_FAILURE;
    }

    std::string preprocessed_file = forbcc::make_temp(argv[1], ".forbpp");

    forbcc::preprocessor prep(argv[1], preprocessed_file);

    prep.execute();

    forbcc::parser parser(preprocessed_file);

    parser.execute();


    forbcc::code_ostream out{std::cout};

    // TODO: modify from here
    std::cout << "====== DECLARATION ======" << std::endl;

    forbcc::module::global_module->print_declaration(out);

    std::cout << "====== DEFINITION ======" << std::endl;

    forbcc::module::global_module->print_definition(out);

    return EXIT_SUCCESS;
}