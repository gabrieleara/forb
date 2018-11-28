//
// Created by gabriele on 28/11/18.
//

#include <iostream>

#include <cstdlib> // mkstemp
#include <cstring> // c strings operations
#include <cassert> // assert
#include <unistd.h> // close

#include <limits>

#include "file_utils.hpp"

// For documentation, see corresponding header file

std::string forbcc::make_temp(const std::string &in_filename, const std::string &extension) {
    const char tmp[] = "/tmp/";
    const char rnd[] = ".XXXXXX";

    const std::string::size_type tmp_length = ::strlen(tmp);
    const std::string::size_type rnd_length = ::strlen(rnd);

    // The name of the temp file that has been created
    std::string temp_name;

    // The basename of the file
    std::string base = in_filename;

    // A temporary buffer for the basename
    char *buffer = new char[in_filename.length()];

    ::strcpy(buffer, in_filename.substr(0, in_filename.find_last_of('.')).c_str());

    // Extracting the basename from the file name
    char *buffer2 = ::basename(buffer);
    base = buffer2;
    delete[] buffer;

    // Now I need to generate the temporary file name
    buffer = new char[tmp_length + base.length() + rnd_length + extension.length() + 1];

    std::string::size_type start = 0;

    // Constructing the base string that will be used by mkstemps
    strcpy(buffer + start, tmp);
    start += tmp_length;

    strcpy(buffer + start, base.c_str());
    start += base.length();

    strcpy(buffer + start, rnd);
    start += rnd_length;

    strcpy(buffer + start, extension.c_str());

    assert((extension.length() <= std::numeric_limits<int>::max() && "The resulting name is too long!"));

    // Now that the string has been constructed, let's make the system call
    int fd = mkstemps(buffer, extension.length());

    if (fd < 0) {
        std::cerr << strerror(errno) << std::endl;

        assert((false));

        return "";
    }

    // We don't need the file descriptor
    ::close(fd);

    temp_name = buffer;

    delete[] buffer;

    return temp_name;
}

void forbcc::remove(const std::string &filename) {
    ::remove(filename.c_str());
}
