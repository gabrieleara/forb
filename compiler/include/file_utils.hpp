//
// Created by gabriele on 28/11/18.
//

#ifndef FORBCC_FILE_UTIL_H
#define FORBCC_FILE_UTIL_H

#include <string>

namespace forbcc {
    /// Extracts the dirname from a path string (UNIX-specific)
    std::string dir_name(const std::string &filename);

    /// Extracts the basename from a path string (UNIX-specific)
    std::string base_name(const std::string &filename);

    /// Removes the extension from a path string (UNIX-specific)
    std::string remove_extension(const std::string &filename);

    /// Returns the extension from a path string (UNIX-specific)
    std::string get_extension(const std::string &filename);

    /// Creates a temporary file starting from the file name given as input using a specific syntax.
    /// The output file will be stored in /tmp and it will have a filename with the following syntax:
    /// basename_of_in_filename_without_extension.XXXXXXextension.
    /// where XXXXXX is a randomly generated set of 6 letters. See man mkstemps for further details.
    std::string make_temp(const std::string &in_filename, const std::string &extension = "");

    /// Deletes a file.
    void remove(const std::string &filename);
}

#endif //FORBCC_FILE_UTIL_H
