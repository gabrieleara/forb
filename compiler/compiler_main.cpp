//
// Created by gabriele on 28/11/18.
//

#include <iostream>
#include "code_ostream.hpp"
#include "preprocessor.hpp"
#include "parser.hpp"

#include "module.hpp"

#include "file_utils.hpp"

// TODO: change this and include exception handling

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

    std::cout << "====== DECLARATION ======" << std::endl;

    forbcc::module::global_module->print_declaration(out);

    std::cout << "====== DEFINITION ======" << std::endl;

    forbcc::module::global_module->print_definition(out);

    return EXIT_SUCCESS;
}