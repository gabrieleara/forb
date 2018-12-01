//
// Created by gabriele on 01/12/18.
//

#include "generator.hpp"

#include <algorithm>         // std::transform
#include <fstream>
#include "file_utils.hpp"
#include "module.hpp"
#include "code_ostream.hpp"

/// Converts a string to uppercase
static inline std::string str_toupper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::toupper(c); });
    return s;
}

/// Prints the header file of the compiled module
static inline void print_header(const std::string &fname_header) {
    std::string define_header = str_toupper(fname_header);

    std::replace(define_header.begin(), define_header.end(), '.', '_');

    std::ofstream        out_stream{fname_header};
    forbcc::code_ostream out_header{out_stream};

    out_header << "#ifndef " << define_header << std::endl
               << "#define " << define_header << std::endl
               << std::endl
               << "#include <forb/forb.hpp>" << std::endl
               << std::endl;

    forbcc::module::global_module->print_declaration(out_header);

    out_header << "#endif // " << define_header << std::endl;
}

/// Prints the source file of the compiled module
static inline void print_source(const std::string &fname_header, const std::string &fname_source) {
    std::ofstream        stream = std::ofstream{fname_source};
    forbcc::code_ostream out{stream};

    out << "#include \"" << fname_header << "\"" << std::endl
        << std::endl
        << "using ssocket = forb::streams::socket;" << std::endl
        << "using shared_memory = forb::streams::shared_memory;" << std::endl
        << "using stream = forb::streams::stream;" << std::endl
        << std::endl;

    forbcc::module::global_module->print_definition(out);
}

// For the rest of documentation, see corresponding header file

void forbcc::generator::execute() const {
    std::string simple_name  = remove_extension(
            base_name(_filename)
            );
    std::string fname_source = simple_name + ".cpp";
    std::string fname_header = simple_name + ".hpp";

    print_header(fname_header);
    print_source(fname_header, fname_source);
}