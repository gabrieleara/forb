//
// Created by gabriele on 20/11/18.
//

#include "preprocessor.hpp"

#include <iostream>
#include <cstring>
#include <exception.hpp>
#include <file_utils.hpp>

int main(int argc, char *argv[]) {
    std::string in_filename;
    std::string out_filename = "a.forbpp";
    int         in_index     = 1;

    if (argc < 1) {
        std::cout << "FORB Pre-Processor" << std::endl
                  << "Usage: forb [-o OUTPUT_FILE=\"a.forbpp\"] INPUT_FILE" << std::endl
                  << std::endl;
        return EXIT_FAILURE;
    }

    if (argc > 1 && ::strcmp(argv[1], "-h") == 0) {
        std::cout << "FORB Pre-Processor" << std::endl
                  << "Usage: forb [-o OUTPUT_FILE=\"a.forbpp\"] INPUT_FILE" << std::endl
                  << std::endl;
        return EXIT_SUCCESS;
    }

    try {
        for (; in_index < argc && ::strcmp(argv[in_index], "-o") == 0; in_index += 2) {
            out_filename = argv[in_index + 1];
        }

        if (in_index >= argc) {
            throw forbcc::exception{"parsing command-line arguments", "missing input file name argument "};
        }

        in_filename = argv[in_index];

        for (++in_index; in_index < argc && ::strcmp(argv[in_index], "-o") == 0; in_index += 2) {
            out_filename = argv[in_index + 1];
        }

        std::string extension = forbcc::get_extension(in_filename);
        if (extension != "forb") {
            throw forbcc::exception{"parsing command-line arguments", "invalid file extension \"" + extension + "\""};
        }

        forbcc::preprocessor p{in_filename, out_filename};
        p.execute();
    } catch (forbcc::exception &ex) {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

