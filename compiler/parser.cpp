//
// Created by gabriele on 19/11/18.
//

#include <type_traits>
#include <fstream>
#include <string>
#include <regex>

#include "parser.hpp"

#include "module.hpp"
#include "interface.hpp"

#include "variable.hpp"
#include "parameter.hpp"
#include "method.hpp"

#include "types/type.hpp"
#include "types/type_array.hpp"
#include "types/type_struct.hpp"
#include "types/type_primitive.hpp"

const std::string forbcc::keywords[]           = {"MODULE", "TYPE", "INTERFACE"};
const std::string forbcc::direction_keywords[] = {"IN", "OUT", "INOUT"};

/// Converts a string to uppercase.
/// See https://en.cppreference.com/w/cpp/string/byte/toupper for further reference.
static inline std::string str_toupper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::toupper(c); });
    return s;
}

/// Converts given string into a direction.
/// Throws an exception on failure.
forbcc::direction str_to_direction(std::string str) {
    str        = str_toupper(str);
    for (int i = 0; i < 3; ++i) {
        if (str == forbcc::direction_keywords[i])
            return static_cast<forbcc::direction>(i);
    }

    throw 19;
}

template<typename T>
using underl_type = typename std::underlying_type<T>::type;

template<typename T>
static underl_type<T> to_int(T enum_val) {
    return static_cast<underl_type<T>>(enum_val);
}

static inline void next(std::istream &file, std::string &str) {
    if (!(file >> str)) {
        // TODO: Unexpected EOF?
        throw 15;
    }
}

static inline void skip_symbol(std::istream &file, const std::string &symbol) {
    std::string str;

    next(file, str);

    if (str != symbol)
        throw 17;
}

// TODO: change to a regex that doesn't match invalid names for forb
static inline bool is_valid(const std::string &name) {
    return std::regex_match(name, std::regex("([a-zA-Z_][a-zA-Z0-9_]*)"));
}

/* ***************************************** TYPE ***************************************** */

static inline std::shared_ptr<const forbcc::type> find_type(
        const forbcc::module::ptr_t &current_module,
        const std::string &name) {
    std::shared_ptr<const forbcc::type> type_ptr;

    if (forbcc::type_primitive::known_types.contains(name)) {
        // It is a primitive type
        type_ptr = forbcc::type_primitive::known_types[name];
    } else {
        // It is a struct type, which shall be either defined within this module or in another module
        // visible from this module
        type_ptr = current_module->find_struct(name);
    }

    // If at this point type_ptr equals nullptr, the type is unknown
    if (type_ptr == nullptr) {
        throw 57;
    }

    return type_ptr;
}

static inline forbcc::type_array::ptr_const_t parse_type_array(
        std::istream &file,
        std::shared_ptr<const forbcc::type> &item_type) {
    forbcc::type_array::ptr_const_t array_ptr;
    forbcc::type_array::length_t    length;
    std::string                     input_str;
    std::string                     array_id;

    next(file, input_str);

    length = stol(input_str);

    if (length < 1) {
        throw 19;
    }

    skip_symbol(file, "]");

    array_id = forbcc::type_array::to_identifier(item_type, length);

    if (forbcc::type_array::arrays.contains(array_id)) {
        array_ptr = forbcc::type_array::arrays.operator[](array_id);
        return array_ptr;
    } else {
        // I have to create a new array type
        array_ptr = forbcc::type_array::new_ptr_const(item_type, length);
        forbcc::type_array::arrays.insert(array_id, array_ptr);
        return array_ptr;
    }
}

/* ***************************************** TYPE END ***************************************** */

/* ***************************************** VARIABLE ***************************************** */

/// Delimiter can be either ';' or ',' or ')', you can even give multiple characters in a list
/// TODO: detailed description
static forbcc::variable parse_variable(std::istream &file,
                                       const forbcc::module::ptr_t &current_module,
                                       const std::string &type_str,
                                       const std::string &delimiters,
                                       std::string::size_type &delimiter_index) {

    std::shared_ptr<const forbcc::type> var_type;
    std::string                         var_name;
    std::string                         input_str;

    var_type = find_type(current_module, type_str);

    // Now I need to get the variable's name
    next(file, var_name);

    // Then I need to get a symbol, either one of the delimiter characters or [ for an array
    next(file, input_str);

    if (input_str == "[") {
        // It's actually an array!
        var_type = parse_type_array(file, var_type);

        next(file, input_str);
    }

    delimiter_index = delimiters.find(input_str[0]);

    if (input_str.length() != 1 && delimiter_index >= delimiters.length()) {
        // Unexpected symbol
        throw 45;
    }

    return forbcc::variable{var_type, var_name};
}

static inline forbcc::variable parse_variable(std::istream &file,
                                              const forbcc::module::ptr_t &current_module,
                                              const std::string &type_name,
                                              const std::string &delimiters) {
    std::string::size_type dummy_index;
    return parse_variable(file, current_module, type_name, delimiters, dummy_index);
}

/* ***************************************** VARIABLE END ***************************************** */

/* ***************************************** STRUCT ***************************************** */

static inline forbcc::type_struct::ptr_t
struct_declaration(std::istream &file, const forbcc::module::ptr_t &parent_module) {
    forbcc::type_struct::ptr_t ptr;
    std::string                name;

    next(file, name);

    skip_symbol(file, "{");

    if (!is_valid(name)) {
        throw 37;
    }

    if (parent_module->contains(name)) {
        // The symbol is already defined within the same module!
        throw 23;
    }

    // A new entity must be created
    ptr = forbcc::type_struct::new_ptr(parent_module, name);
    parent_module->insert(name, ptr);

    return ptr;
}


static void parse_struct(std::istream &file, const forbcc::module::ptr_t &current_module) {
    forbcc::type_struct::ptr_t new_struct;
    forbcc::variable           var;
    std::string                input_str;
    bool                       keep_going = true;

    new_struct = struct_declaration(file, current_module);

    while (keep_going) {
        next(file, input_str);

        // Expecting either a closed } or a variable declaration
        if (input_str == "}") {
            keep_going = false;
            continue;
        }

        // Expecting now a variable declaration
        var = parse_variable(file, current_module, input_str, ";");

        if (!new_struct->insert(var.name(), var)) {
            throw 15;
        }
    }
}

/* ***************************************** STRUCT END ***************************************** */

/* ***************************************** METHOD ***************************************** */

static inline forbcc::parameter parse_parameter(std::istream &file,
                                                const forbcc::module::ptr_t &current_module,
                                                std::string &input_str,
                                                bool &keep_going) {
    forbcc::direction      dir;
    forbcc::variable       var;
    std::string::size_type index;

    dir = str_to_direction(input_str);

    // I need to read the type of the variable before calling parse_variable
    // since parse_variable has been defined to be mainly used for struct
    // declarations
    next(file, input_str);

    var = parse_variable(file, current_module, input_str, ",)", index);

    keep_going = index != 1;

    return forbcc::parameter{dir, var};
}

static inline forbcc::method parse_method(std::istream &file,
                                          const forbcc::module::ptr_t &current_module,
                                          const forbcc::interface::ptr_t &current_interface,
                                          const std::string &return_type_name) {

    forbcc::parameter                   param;
    forbcc::method                      new_method;
    std::shared_ptr<const forbcc::type> return_type;
    std::string                         input_str;
    bool                                keep_going = true;

    return_type = find_type(current_module, return_type_name);

    // Read method name
    next(file, input_str);

    new_method = forbcc::method{current_interface, input_str, return_type};

    skip_symbol(file, "(");

    while (keep_going) {
        next(file, input_str);

        // Expecting either a closed ) or a parameter declaration
        if (input_str == ")") {
            // Stop parsing, expected a new parameter, but parsed ')' instead
            throw 17;
        }

        // Expecting now a parameter declaration, the input_str is expected to be
        // the direction of the parameter, which may be followed by either a `,`
        // or a `)` (which marks the end of this loop).
        param = parse_parameter(file, current_module, input_str, keep_going);

        if (new_method.contains(param.name())) {
            // Already existing parameter name
            throw 15;
        }

        new_method.insert(param.name(), param);
    }

    skip_symbol(file, ";");

    return new_method;
}

/* ***************************************** METHOD END ***************************************** */

/* ***************************************** INTERFACE ***************************************** */


static inline forbcc::interface::ptr_t
interface_declaration(std::istream &file, const forbcc::module::ptr_t &parent_module) {
    forbcc::interface::ptr_t ptr;
    std::string              name;

    next(file, name);

    skip_symbol(file, "{");

    if (!is_valid(name)) {
        throw 37;
    }

    if (parent_module->contains(name)) {
        // The symbol is already defined within the same module!
        throw 23;
    }

    // A new entity must be created
    ptr = forbcc::interface::new_ptr(parent_module, name);
    parent_module->insert(name, ptr);

    return ptr;
}

static void parse_interface(std::istream &file, const forbcc::module::ptr_t &current_module) {
    forbcc::interface::ptr_t new_interface;
    forbcc::method           new_method;
    std::string              input_str;
    bool                     keep_going = true;

    new_interface = interface_declaration(file, current_module);

    while (keep_going) {
        next(file, input_str);

        // Expecting either a closed } or a variable declaration
        if (input_str == "}") {
            keep_going = false;
            continue;
        }

        // Expecting now a variable declaration
        new_method = parse_method(file, current_module, new_interface, input_str);

        if (!new_interface->insert(new_method.id(), new_method)) {
            throw 19;
        }
    }
}

/* ***************************************** INTERFACE END ***************************************** */


/* ***************************************** MODULE ***************************************** */

static forbcc::module::ptr_t module_declaration(std::istream &file, const forbcc::module::ptr_t &parent);

static void parse_module(std::istream &file, const forbcc::module::ptr_t &parent_module);

static void parse_module_body(std::istream &file, const forbcc::module::ptr_t &current_module);

static inline forbcc::module::ptr_t module_declaration(std::istream &file, const forbcc::module::ptr_t &parent) {
    forbcc::module::ptr_t new_module;
    std::string           name;

    next(file, name);

    skip_symbol(file, "{");

    if (!is_valid(name)) {
        throw 37;
    }

    if (parent->contains(name)) {
        new_module = parent->get_module(name);

        if (new_module == nullptr) {
            // The symbol is already defined and it does not refer to a module!
            throw 23;
        }
    } else {
        // A new entity must be created
        new_module = forbcc::module::new_ptr(parent, name);
        parent->insert(name, new_module);
    }

    return new_module;
}

static inline void parse_module_body(std::istream &file, const forbcc::module::ptr_t &current_module) {
    std::string input_str;
    bool        keep_going = true;

    // Expecting either a module, struct or interface keyword
    while (keep_going && file >> input_str) {

        // Keywords are converted to uppercase to ignore case during comparison
        input_str = str_toupper(input_str);

        if (input_str == forbcc::keywords[to_int(forbcc::keywords_idx_t::MODULE)]) {
            parse_module(file, current_module);

        } else if (input_str == forbcc::keywords[to_int(forbcc::keywords_idx_t::STRUCT)]) {
            parse_struct(file, current_module);

        } else if (input_str == forbcc::keywords[to_int(forbcc::keywords_idx_t::INTERFACE)]) {
            parse_interface(file, current_module);

        } else if (input_str == "}") {
            if (current_module == forbcc::module::global_module) {
                throw 17;
            }

            keep_going = false;
        } else {
            throw 42;
        }
    }

    if (!file && current_module != forbcc::module::global_module) {
        // Unexpected end of file
        throw 73;
    }
}

static void parse_module(std::istream &file, const forbcc::module::ptr_t &parent_module) {
    forbcc::module::ptr_t new_module = module_declaration(file, parent_module);

    parse_module_body(file, new_module);
}

/* ***************************************** MODULE END ***************************************** */

void forbcc::parser::execute() {
    std::ifstream file{filename};

    forbcc::module::ptr_t new_module = forbcc::module::global_module;

    parse_module_body(file, new_module);
}