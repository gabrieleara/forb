//
// Created by gabriele on 19/11/18.
//

#include <type_traits>
#include <fstream>
#include <string>
#include <regex>

#include <parser.hpp>

#include <exception.hpp>
#include <module.hpp>
#include <interface.hpp>
#include <parameter.hpp>
#include <method.hpp>
#include <variable.hpp>

#include <types/type.hpp>
#include <types/type_array.hpp>
#include <types/type_struct.hpp>
#include <types/type_primitive.hpp>

/*
 ***********************************************************************************************************************
 *                                 STATIC FUNCTIONS DECLARATIONS (WITH DOCUMENTATION)                                  *
 ***********************************************************************************************************************
 */

// Following declarations are split in sections, starting from commonly used functions, then parsers for each
// different entity used within FORB IDL language.

/// Converts a string to uppercase.
/// See https://en.cppreference.com/w/cpp/string/byte/toupper for further reference.
static inline std::string str_toupper(std::string s);

/// Converts given string into a direction, throwing an exception on failure.
static inline forbcc::direction str_to_direction(std::string str);

/// The integer type underlying an enum class type.
template<typename T>
using underl_type = typename std::underlying_type<T>::type;

/// Converts an enum to a bare integer type.
template<typename T>
static inline underl_type<T> to_int(T enum_val) {
    return static_cast<underl_type<T>>(enum_val);
}

/// Reads the next string from file, throwing an exception when in input error occurs.
static inline void next(std::istream &file, std::string &str);

/// Skips next string, but only if it corresponds to the given symbol, otherwise throws an exception.
static inline void skip_symbol(std::istream &file, const std::string &symbol);

/// Returns true if the given name is a valid C++ name that can be used for FORB library components.
static inline bool is_valid(const std::string &name);

/* ***************************************** TYPE ***************************************** */

/// Returns a pointer to the type specified by name, searching within current_module scope of visibility.
/// If the given name is a primitive type, the corresponding primitive type pointer is returned.
/// Otherwise, see forbcc::module::find_struct for further details.
/// On failure this function throws an exception.
static inline std::shared_ptr<const forbcc::type> find_type(
        const forbcc::module::ptr_t &current_module,
        const std::string &name);

/// Parses an array declaration, to be called from within parse_type.
/// This function expects as next string the length of the array, which means
/// that externally the `[` symbol should be already parsed, indicating that
/// this function shall be called.
/// Returns either a pointer to the array or throws an exception on error.
static inline forbcc::type_array::ptr_const_t parse_type_array(
        std::istream &file,
        std::shared_ptr<const forbcc::type> &item_type);

/* ***************************************** TYPE END ***************************************** */

/* ***************************************** VARIABLE ***************************************** */

/// Parses a variable declaration. The variable declaration shall be terminated by one of the
/// characters contained in delimiters string (usually `;`, `,` or `)`.
/// The delimiter_index argument is an output argument and it will be updated with the index
/// of the matching delimiter character within delimiters string.
/// The type of the variable is looked up starting from the type_str string.
static inline forbcc::variable parse_variable(std::istream &file,
                                              const forbcc::module::ptr_t &current_module,
                                              const std::string &type_str,
                                              const std::string &delimiters,
                                              std::string::size_type &delimiter_index);

/// Same as the other parse_variable, but without the index checking.
/// Also this function throws exceptions on failure.
static inline forbcc::variable parse_variable(std::istream &file,
                                              const forbcc::module::ptr_t &current_module,
                                              const std::string &type_name,
                                              const std::string &delimiters);

/* *************************************** VARIABLE END *************************************** */

/* **************************************** STRUCTURE ***************************************** */

/// Parses the name of a structure and inserts the structure to the current_module set of entities.
/// Throws an exception on failure.
static inline forbcc::type_struct::ptr_t struct_declaration(
        std::istream &file,
        const forbcc::module::ptr_t &parent_module);

/// Parses a structure declaration.
/// Expects the next symbol to be the structure name, followed by `{`, a list of variables and `}`, followed by `;`
static inline void parse_struct(std::istream &file, const forbcc::module::ptr_t &current_module);

/* ************************************** STRUCTURE END ************************************* */

/* ***************************************** METHOD ***************************************** */

/// Parses the declaration of a method parameter, expects input_str to be the direction specifier,
/// while following there is a variable declaration, terminated by either , or ).
/// Returns the parsed parameter.
/// The current_module argument is needed to perform type lookup with scope.
/// The keep_going argument is an output argument and it specifies whether `)` was reached or not,
/// thus whether the parameter list is finished or not.
static inline forbcc::parameter parse_parameter(std::istream &file,
                                                const forbcc::module::ptr_t &current_module,
                                                std::string &input_str,
                                                bool &keep_going);

/// Parses a method declaraton; the return_type_name parameter being the already-parsed return type of the method.
/// Performs type lookup using current_module as scope for the return type, then parses the parameter list of the
/// method (if one or more parameters are present).
/// Returns the newly parsed method, throws an exception on parse failure.
static inline forbcc::method parse_method(std::istream &file,
                                          const forbcc::module::ptr_t &current_module,
                                          const forbcc::interface::ptr_t &current_interface,
                                          const std::string &return_type_name);

/* *************************************** METHOD END *************************************** */

/* *************************************** INTERFACE **************************************** */

/// Parses the interface name and creates an empty interface within current_module,
/// appending it to the module.
/// Returns a pointer to the newly created interface, or throws an exception on parse failure.
static inline forbcc::interface::ptr_t interface_declaration(
        std::istream &file,
        const forbcc::module::ptr_t &current_module);

/// Parses an interface declaration.
/// The current module is used both to insert the newly created interface in its children set and
/// to perform name lookup for referred types.
/// Expects the interface name as next symbol, followed by `{`, a list of methods, `}' and finally
/// a `;` to delimit the end of the declaration.
static inline void parse_interface(std::istream &file, const forbcc::module::ptr_t &current_module);


/* ************************************* INTERFACE END ************************************** */

/* ***************************************** MODULE ***************************************** */

/// Parses the name of the module and creates a new module (if no such module was already defined
/// within parent) or returns a previously referred module.
/// On error an exception is thrown.
static inline forbcc::module::ptr_t module_declaration(std::istream &file, const forbcc::module::ptr_t &parent);

/// Parses the body of a module, which may contain new modules, structures or interfaces.
/// On error an exception is thrown.
static void parse_module_body(std::istream &file, const forbcc::module::ptr_t &current_module);

/// Parses a module, expects next symbol to be the module name, followed by `{`, the body of the module
/// and finally a `}`. Only forbcc::module::global_module is supposed not to have a `}` at the end, but
/// expects the end of the input stream instead.
/// On error an exception is thrown.
static void parse_module(std::istream &file, const forbcc::module::ptr_t &parent_module);

/* *************************************** MODULE END *************************************** */


/*
 ***********************************************************************************************************************
 *                                                   IMPLEMENTATION                                                    *
 ***********************************************************************************************************************
 */

static inline std::string str_toupper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::toupper(c); });
    return s;
}

static inline forbcc::direction str_to_direction(std::string str) {
    str = str_toupper(str);

    for (int i = 0; i < 3; ++i) {
        if (str == forbcc::direction_keywords[i])
            return static_cast<forbcc::direction>(i);
    }

    throw forbcc::exception{"parse of input file", "expected direction qualifier, got \"" + str + "\" instead"};
}

static inline void next(std::istream &file, std::string &str) {
    if (!(file >> str)) {
        throw forbcc::exception{"parse of input file", "unexpected EOF"};
    }
}

static inline void skip_symbol(std::istream &file, const std::string &symbol) {
    std::string str;

    next(file, str);

    if (str != symbol)
        throw forbcc::exception{"parse of input file",
                                "unexpected symbol \"" + str + "\" encountered while expecting '" + symbol + "'"};
}

static inline bool is_valid(const std::string &name) {
    if (!std::regex_match(name, std::regex("([a-zA-Z_][a-zA-Z0-9_]*)")))
        return false;

    for (int i = 0; i < forbcc::reserved_names_length; ++i) {
        if (forbcc::reserved_names[i] == name)
            return false;
    }

    return true;
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
        throw forbcc::exception{"parse of input file", "unknown type identifier \"" + name + "\""};
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

    try {
        length = stol(input_str);
    } catch (std::invalid_argument ex) {
        length = -1;
    }

    if (length < 1) {
        throw forbcc::exception{"parse of input file",
                                "expected strictly positive integer as array length, parsed \"" + input_str +
                                "\" instead"};
    }

    skip_symbol(file, "]");

    array_id = forbcc::type_array::to_identifier(item_type, length);

    if (forbcc::type_array::arrays.contains(array_id)) {
        // The parsed array is already defined somewhere
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

static inline forbcc::variable parse_variable(std::istream &file,
                                              const forbcc::module::ptr_t &current_module,
                                              const std::string &type_str,
                                              const std::string &delimiters,
                                              std::string::size_type &delimiter_index) {

    std::shared_ptr<const forbcc::type> var_type;
    std::string                         var_name;
    std::string                         input_str;

    // Finding the type of the variable
    var_type = find_type(current_module, type_str);

    // Now I need to get the variable's name
    next(file, var_name);

    if (!is_valid(var_name)) {
        throw forbcc::exception{"parse of input file", "name \"" + var_name + "\" is not a valid name"};
    }

    // Then I need to get a symbol, either one of the delimiter characters or [ for an array
    next(file, input_str);

    if (input_str == "[") {
        // It's actually an array! Parse its length and update the type of the variable
        var_type = parse_type_array(file, var_type);

        next(file, input_str);
    }

    delimiter_index = delimiters.find(input_str[0]);

    if (input_str.length() != 1 && delimiter_index >= delimiters.length()) {
        throw forbcc::exception{"parse of input file",
                                "unexpected symbol in variable declaration '" + input_str +
                                "', was expecting one of the following symbols: \"" +
                                delimiters + "\""};
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

/* *************************************** VARIABLE END *************************************** */

/* **************************************** STRUCTURE ***************************************** */

static inline forbcc::type_struct::ptr_t struct_declaration(
        std::istream &file,
        const forbcc::module::ptr_t &parent_module) {
    forbcc::type_struct::ptr_t ptr;
    std::string                name;

    /// Parse the structure name
    next(file, name);

    skip_symbol(file, "{");

    if (!is_valid(name)) {
        throw forbcc::exception{"parse of input file", "name \"" + name + "\" is not a valid name"};
    }

    if (parent_module->contains(name)) {
        // The symbol is already defined within the same module!
        throw forbcc::exception{"parse of input file",
                                "name \"" + name + "\" already exists within module \"" + parent_module->codename() +
                                "\""};
    }

    // A new entity must be created
    ptr = forbcc::type_struct::new_ptr(parent_module, name);
    parent_module->insert(name, ptr);

    return ptr;
}

static inline void parse_struct(std::istream &file, const forbcc::module::ptr_t &current_module) {
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

        if (new_struct->contains(var.name())) {
            throw forbcc::exception{"parse of input file",
                                    "attribute \"" + var.name() + "\" already defined for \"" + new_struct->codename() +
                                    "\""};
        }

        new_struct->insert(var.name(), var);
    }

    // Expects `;` to terminate declaration
    skip_symbol(file, ";");

    if (new_struct->list().empty()) {
        throw forbcc::exception{"parse of input file", "structure \"" + new_struct->codename() + "\" is empty"};
    }
}

/* ************************************** STRUCTURE END *************************************** */

/* ****************************************** METHOD ****************************************** */

static inline forbcc::parameter parse_parameter(std::istream &file,
                                                const forbcc::module::ptr_t &current_module,
                                                std::string &input_str,
                                                bool &keep_going) {
    forbcc::direction      dir;
    forbcc::variable       var;
    std::string::size_type index;

    dir = str_to_direction(input_str);

    // I need to read the type of the variable before calling parse_variable
    // since parse_variable has been defined to be used both for structure
    // and method declarations.
    next(file, input_str);

    // The index is used to check whether `)` delimiter was found or not
    var = parse_variable(file, current_module, input_str, ",)", index);

    // If `)` is reached, then the parameter list is finished
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
    bool                                keep_going     = true;
    bool                                has_parameters = false;

    // Get the return type from the input string
    return_type = find_type(current_module, return_type_name);

    // Read method name
    next(file, input_str);

    new_method = forbcc::method{current_interface, input_str, return_type};

    skip_symbol(file, "(");

    while (keep_going) {
        next(file, input_str);

        // Expecting either a `)` or a parameter declaration
        if (input_str == ")") {
            if (has_parameters) {
                // Stop parsing, expected a new parameter, but parsed ')' instead
                throw forbcc::exception{"parse of input file",
                                        "expected a parameter declaration for method \"" +
                                        current_interface->codename() + "." + new_method.name() +
                                        "\", but the ')' symbol was reached instead"};
            }

            // The method has no parameters
            keep_going = false;
            continue;
        }

        // Expecting now a parameter declaration, the input_str is expected to be
        // the direction of the parameter, which may be followed by either a `,`
        // or a `)` (which marks the end of this loop).
        param = parse_parameter(file, current_module, input_str, keep_going);

        if (new_method.contains(param.name())) {
            // Already existing parameter name
            throw forbcc::exception{"parse of input file",
                                    "parameter \"" + param.name() + "\" is already defined for method \"" +
                                    current_interface->codename() + "." + new_method.name() + "\""
            };
        }

        new_method.insert(param.name(), param);
        has_parameters = true;
    }

    // Expects `;` to terminate declaration
    skip_symbol(file, ";");

    return new_method;
}

/* **************************************** METHOD END **************************************** */

/* **************************************** INTERFACE ***************************************** */

static inline forbcc::interface::ptr_t interface_declaration(
        std::istream &file,
        const forbcc::module::ptr_t &current_module) {
    forbcc::interface::ptr_t ptr;
    std::string              name;

    next(file, name);

    skip_symbol(file, "{");

    if (!is_valid(name)) {
        throw forbcc::exception{"parse of input file", "name \"" + name + "\" is not a valid name"};
    }

    if (current_module->contains(name)) {
        // The symbol is already defined within the same module!
        throw forbcc::exception{"parse of input file",
                                "entity \"" + name + "\" is already defined in module \"" + current_module->codename() +
                                "\""};
    }

    // A new entity must be created
    ptr = forbcc::interface::new_ptr(current_module, name);
    current_module->insert(name, ptr);

    return ptr;
}

static inline void parse_interface(std::istream &file, const forbcc::module::ptr_t &current_module) {
    forbcc::interface::ptr_t new_interface;
    forbcc::method           new_method;
    std::string              input_str;
    bool                     keep_going = true;

    /// Parses the name of the interface and returns the new interface.
    new_interface = interface_declaration(file, current_module);

    while (keep_going) {
        next(file, input_str);

        // Expecting either a `}` or a method declaration
        if (input_str == "}") {
            keep_going = false;
            continue;
        }

        // Expecting now a variable method
        new_method = parse_method(file, current_module, new_interface, input_str);

        if (new_interface->contains(new_method.id())) {
            // A method with same name and parameters overload already exists
            throw forbcc::exception{"parse of input file",
                                    "invalid overload of method \"" + new_interface->codename() + "." +
                                    new_method.name() + "\", the same exact signature is already present"};
        }

        new_interface->insert(new_method.id(), new_method);
    }

    // Expects `;` to terminate declaration
    skip_symbol(file, ";");

    if (new_interface->list().empty()) {
        throw forbcc::exception{"parse of input file", "interface \"" + new_interface->codename() + "\" is empty"};
    }
}

/* ************************************** INTERFACE END *************************************** */

/* ****************************************** MODULE ****************************************** */

static inline forbcc::module::ptr_t module_declaration(std::istream &file, const forbcc::module::ptr_t &parent) {
    forbcc::module::ptr_t new_module;
    std::string           name;

    next(file, name);

    skip_symbol(file, "{");

    if (!is_valid(name)) {
        throw forbcc::exception{"parse of input file", "name \"" + name + "\" is not a valid name"};
    }

    if (parent->contains(name)) {
        new_module = parent->get_module(name);

        if (new_module == nullptr) {
            // The symbol is already defined and it does not refer to a module!
            throw forbcc::exception{"parse of input file",
                                    "invalid declaration of \"" + name + "\", symbol already defined within module \"" +
                                    parent->codename() + "\""};
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
                throw forbcc::exception{"parse of input file",
                                        "unexpected '}' in global module, expected EOF instead"};
            }

            skip_symbol(file, ";");

            keep_going = false;
        } else {
            throw forbcc::exception{"parse of input file",
                                    "unexpected symbol \"" + input_str + "\", expected a keyword instead"};
        }
    }

    if (!file && current_module != forbcc::module::global_module) {
        // Unexpected end of file
        throw forbcc::exception{"parse of input file",
                                "unexpected EOF before reaching the end of module \"" + current_module->codename() +
                                "\""};
    }
}

static void parse_module(std::istream &file, const forbcc::module::ptr_t &parent_module) {
    forbcc::module::ptr_t new_module = module_declaration(file, parent_module);

    parse_module_body(file, new_module);
}

/* **************************************** MODULE END **************************************** */

/*
 ***********************************************************************************************************************
 *                                             PARSER CLASS IMPLEMENTATION                                             *
 ***********************************************************************************************************************
 */

// For documentation, see corresponding header file

/* ******************************************* STATIC VARIABLES/CONSTANTS ******************************************* */

const std::string forbcc::keywords[]           = {"MODULE", "STRUCT", "INTERFACE"};
const std::string forbcc::direction_keywords[] = {"IN", "OUT", "INOUT"};
const std::string forbcc::reserved_names[]     = {"datastream", "callstream", "code"};
const int forbcc::reserved_names_length = 2;

/* **************************************************** METHODS ***************************************************** */

void forbcc::parser::execute() {
    std::ifstream file{filename};

    forbcc::module::ptr_t new_module = forbcc::module::global_module;

    parse_module_body(file, new_module);
}