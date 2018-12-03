//
// Created by gabriele on 01/12/18.
//

#ifndef FORBCC_GENERATOR_H
#define FORBCC_GENERATOR_H

#include <string>

namespace forbcc {
    /// Generates source and header files from the (already populated) forbcc::module::global_module.
    class generator {
        /// The name of the file from which the source and header file names will be generated.
        std::string _filename;

    public:
        /// Creates a new generator, the given filename will be used as base for the output header and source files.
        explicit generator(const std::string &filename) : _filename(filename) {};

        /// Generates code header and source files in current directory, it requires forbcc::module::global_module
        /// to be already populated.
        void execute() const;
    };
}


#endif //FORBCC_GENERATOR_H
