//
// Created by gabriele on 20/11/18.
//

#ifndef FORBCC_PREPROCESSOR_H
#define FORBCC_PREPROCESSOR_H

#include <string>

namespace forbcc {
    /// Creates a temporary file starting from the file name given as input using a specific syntax.
    /// The output file will be stored in /tmp and it will have a filename with the following syntax:
    /// basename_of_in_filename_without_extension.XXXXXXextension
    /// where XXXXXX is a randomly generated set of 6 letters. See man mkstemps for further details.
    std::string make_temp(const std::string &in_filename, const std::string &extension = "");

    /// Deletes a file.
    void remove(const std::string &filename);

    /// Pre-processes the given input file and writes its output in the given output file.
    class preprocessor {
        /// The name of the input file
        std::string _in_filename;

        /// The name of the output file
        std::string _out_filename;

    public:
        /// Constructs a new preprocessor instance
        preprocessor(const std::string &in_filename,
                     const std::string &out_filename)
                : _in_filename(in_filename),
                  _out_filename(out_filename) {};

        /// Performs the preprocessing on given files
        void execute() const;
    };
}


#endif //FORBCC_PREPROCESSOR_H
