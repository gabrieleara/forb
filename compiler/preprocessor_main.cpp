//
// Created by gabriele on 20/11/18.
//

#include "preprocessor.hpp"

#include <iostream>
#include <cstring>

int main(int argc, char *argv[]) {
    std::string in_filename;
    std::string out_filename = "a.forbpp";
    int         in_index     = 1;

    if (argc > 1 && ::strcmp(argv[1], "-h") == 0) {
        std::cout << "FORB Pre-Processor\n"
                     "Usage: forb [-o OUTPUT_FILE=\"a.forbpp\"] INPUT_FILE\n"
                  << std::endl;
        return EXIT_SUCCESS;
    }

    for (; in_index < argc && ::strcmp(argv[in_index], "-o") == 0; in_index += 2) {
        out_filename = argv[in_index + 1];
    }

    if (in_index >= argc) {
        std::cerr << "Please, give as argument the name of the file to be pre-processed!" << std::endl;
        return EXIT_FAILURE;
    }

    in_filename = argv[in_index];

    for (++in_index; in_index < argc && ::strcmp(argv[in_index], "-o") == 0; in_index += 2) {
        out_filename = argv[in_index + 1];
    }

    forbcc::preprocessor p{in_filename, out_filename};

    // TODO: Try catch!
    p.execute();

    return EXIT_SUCCESS;

}

