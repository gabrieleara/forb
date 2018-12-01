//
// Created by gabriele on 28/11/18.
//

#include <iostream>

#include "generator.hpp"
#include "code_ostream.hpp"
#include "preprocessor.hpp"
#include "parser.hpp"

#include "module.hpp"
#include "exception.hpp"
#include "file_utils.hpp"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr
                << "Please give as argument the name of the *.forb file to be preprocessed and compiled, "
                   "or simply the *.forbpp file to be compiled."
                << std::endl;
        return EXIT_FAILURE;
    }

    try {
        std::string in_filename = argv[1];
        std::string extension   = forbcc::get_extension(in_filename);
        std::string preprocessed_file;

        if (extension == "forb") {
            // File must be pre-processed first
            preprocessed_file = forbcc::make_temp(argv[1], ".forbpp");

            forbcc::preprocessor preprocessor(argv[1], preprocessed_file);
            preprocessor.execute();

        } else if (extension == "forbpp") {
            preprocessed_file = in_filename;
        } else {
            throw forbcc::exception{"parsing command-line arguments", "invalid file extension \"" + extension + "\""};
        }

        forbcc::parser parser(preprocessed_file);
        parser.execute();

        forbcc::generator generator(argv[1]);
        generator.execute();
    } catch (forbcc::exception &ex) {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}