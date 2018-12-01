//
// Created by gabriele on 28/11/18.
//

#include <iostream>

#include <cstdlib> // mkstemp
#include <cstring> // c strings operations
#include <cassert> // assert
#include <unistd.h> // close
#include <libgen.h> // basename or dirname

#include <limits>
#include <exception.hpp>

#include "file_utils.hpp"

// For documentation, see corresponding header file

/// Executes the given function that may modify a char array on a copy of the given filename.
/// Used by forbcc::dirname and forbcc::basename.
template<typename F>
static std::string extract_name(const std::string &filename, F func) {
    std::string extracted_name;
    char        *buffer;

    buffer = new char[filename.length()];

    std::strcpy(buffer, filename.c_str());

    extracted_name = (*func)(buffer);

    delete[] buffer;

    return extracted_name;
}

std::string forbcc::dir_name(const std::string &filename) {
    return extract_name(filename, ::dirname);
}

std::string forbcc::base_name(const std::string &filename) {
    return extract_name(filename, ::basename);
}

std::string forbcc::remove_extension(const std::string &filename) {
    std::string::size_type index_dot   = filename.find_last_of('.');
    std::string::size_type index_slash = filename.find_last_of('/');

    if (index_dot > index_slash || index_slash == std::string::npos)
        return filename.substr(0, index_dot);

    return filename;
}

std::string forbcc::get_extension(const std::string &filename) {
    std::string::size_type index_dot   = filename.find_last_of('.');
    std::string::size_type index_slash = filename.find_last_of('/');

    if ((index_dot > index_slash || index_slash == std::string::npos) && index_dot < (filename.length() - 1))
        return filename.substr(index_dot + 1, std::string::npos);

    return "";
}

std::string forbcc::make_temp(const std::string &in_filename, const std::string &extension) {
    const char tmp[] = "/tmp/";
    const char rnd[] = ".XXXXXX";

    const std::string::size_type tmp_length = ::strlen(tmp);
    const std::string::size_type rnd_length = ::strlen(rnd);

    // The name of the temp file that has been created
    std::string temp_name;

    // Extract the basename of the file and remove its extension
    std::string base = remove_extension(base_name(in_filename));

    // Now I need to generate the temporary file name
    // The size of the temporary file name
    long temp_fname_size = tmp_length + base.length() + rnd_length + extension.length() + 1;

    // The buffer used to construct the temporary file name
    char *buffer = new char[temp_fname_size];

    // The index from which we copy our strings
    std::string::size_type start = 0;

    // Constructing the base string that will be used by mkstemps
    std::strcpy(buffer + start, tmp);
    start += tmp_length;

    std::strcpy(buffer + start, base.c_str());
    start += base.length();

    std::strcpy(buffer + start, rnd);
    start += rnd_length;

    std::strcpy(buffer + start, extension.c_str());

    // Checking that the extension is valid for mkstemp
    assert((extension.length() <= std::numeric_limits<int>::max() && "Invalid file extension!"));

    // Now that the string has been constructed, let's make the system call
    int fd = ::mkstemps(buffer, extension.length());

    if (fd < 0) {
        // We don't need the file descriptor outside from this function
        ::close(fd);

        std::cerr << strerror(errno) << std::endl;

        assert((false));

        throw forbcc::exception{"preparetion of the preprocessor",
                                "unable to create temporary file \"" + std::string(buffer) + "\""};
    }

    // We don't need the file descriptor outside from this function
    ::close(fd);

    temp_name = buffer;

    delete[] buffer;

    return temp_name;
}

void forbcc::remove(const std::string &filename) {
    ::remove(filename.c_str());
}
