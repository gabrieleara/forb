//
// Created by gabriele on 20/11/18.
//

#include "preprocessor.hpp"

#include <string>
#include <fstream>
#include <sstream>

#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <cassert>

// For documentation of forbcc::preprocessor class, see corresponding header file


/// Creates a temporary file starting from the file name given as input using a specific syntax.
/// The output file will be stored in /tmp and it will have a filename with the following syntax:
/// basename_of_in_filename_without_extension.XXXXXXextension
/// where XXXXXX is a randomly generated set of 6 letters. See man mkstemps for further details.
inline std::string forbcc::make_temp(const std::string &in_filename, const std::string &extension) {
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

    // Now that the string has been constructed, let's make the system call
    int fd = mkstemps(buffer, extension.length());

    if (fd < 0) {
        // Construct an exception with strerror(errno)
        assert(false);
        return "";
    }

    // We don't need the file descriptor
    ::close(fd);

    temp_name = buffer;

    delete[] buffer;

    return temp_name;
}

/// Deletes a file.
inline void forbcc::remove(const std::string &filename) {
    ::remove(filename.c_str());
}

/// Moves the start_index forward till the end of the current multi-line comment, or until it reaches the end of the line.
/// It returns whether the end of the comment was reached or not.
inline bool skip_multiline_comment(const std::string &in_line, std::string::size_type &start_index) {
    bool in_comment = true;

    while (in_comment && start_index < in_line.length()) {
        start_index = in_line.find("*/", start_index);

        if (start_index <= in_line.length()) {
            // No more inside a multi-line comment
            in_comment = false;

            // Skip "*/" substring
            start_index = start_index + 2;
        }
    }

    return in_comment;
}

/// Returns the index of the first character within in_line that corresponds to a character within symbols, starting
/// from the start_index position.
/// It is simply a wrapper of std::string::find_first_of, so check that out for further reference.
inline std::string::size_type find_symbol(const std::string &in_line,
                                          const std::string &symbols,
                                          std::string::size_type start_index) {
    return in_line.find_first_of(symbols, start_index);
}

/// Removes any comment inside in_filename when copying it to out_filename.
/// Comment types supported are // and /* */ C++ style comments.
inline void remove_comments(const std::string &in_filename, const std::string &out_filename) {

    // Input and output streams
    std::ifstream in{in_filename};
    std::ofstream out{out_filename};

    // The line number
    std::string::size_type line_num = 0;

    // Column indexes:
    // - start_index indicates the point that was reached when copying current line
    std::string::size_type start_index;
    // - index indicates the current column
    std::string::size_type index;

    // Input line
    std::string in_line;

    // Keeps track whether current line/column belongs to a multi-line comment
    bool in_comment = false;

    // As long as the file has lines in it
    while (std::getline(in, in_line, in.widen('\n'))) {

        ++line_num;

        // Reset variables
        start_index = 0;

        // If within multi-line comment, try to find '*/' if possible, otherwise keep skipping lines
        if (in_comment) {
            in_comment = skip_multiline_comment(in_line, start_index);
        }

        // Within multi-line comments this for is never executed because start_index >= in_line.length();
        // Moves index forward until a / character is reached, meaning either a // or a /* comment have been reached, or
        // the the end of the line is reached.
        for (index = in_line.find('/', start_index);
             index < in_line.length();
             index = in_line.find('/', start_index)) {

            // There is at least one comment to skip, but copying from the start_index to current index is ok
            out << in_line.substr(start_index, index - start_index);

            // Cannot have a single '/' at the end of the file, it's a syntax error
            if (in_line.length() == index + 1) {
                // "Error parsing line" + line_num + ", unexpected symbol at column" + index + ": '/'."
                throw 17;
            }

            // Next character determines the type of the comment
            switch (in_line[index + 1]) {
                case '/':
                    // This is a line comment, so we just skip the rest of the line
                    start_index = in_line.length();
                    break;
                case '*':
                    // Multiline comment, find next "*/" or skip line
                    start_index = index + 1;

                    // Returns true if "*/" was not found in current line
                    in_comment = skip_multiline_comment(in_line, start_index);
                    break;
                default:
                    // "Error parsing line" + line_num + ", unexpected symbol at column" + index + ": '/'."
                    throw 17;
            }
        }

        // Copy remaining parts of the input line until the end of the line is reached, if not reached already
        // This copy is performed only when comments are nowhere to be found between start_index and the end of the
        // line, so it's safe.
        if (start_index < in_line.length()) {
            out << in_line.substr(start_index);
        }

        // Finally write the output line to output file
        out << std::endl;
    }

    // A multi-line comment was not closed!
    if (in_comment) {
        throw 18;
    }
}

/// Places a space before and after each symbol used in FORB IDL language.
/// Used to simplify the compiler's job to identify names of elements.
inline void expand_symbols(const std::string &in_filename, const std::string &out_filename) {

    // Input and output streams
    std::ifstream in{in_filename};
    std::ofstream out{out_filename};

    // Recognized symbols in FORB IDL language.
    const std::string      symbols  = "{}[]();,";

    // The line number
    std::string::size_type line_num = 0;

    // Column indexes:
    // - start_index indicates the point that was reached when copying current line
    std::string::size_type start_index;
    // - index indicates the current column
    std::string::size_type index;

    // Input line
    std::string in_line;

    // As long as the file has lines in it
    while (std::getline(in, in_line, in.widen('\n'))) {
        ++line_num;

        // Reset start column index
        start_index = 0;

        for (index = find_symbol(in_line, symbols, start_index);
             index < in_line.length();
             index = find_symbol(in_line, symbols, start_index)) {

            // There is at one symbol to expand
            out << in_line.substr(start_index, index - start_index)
                << " " << in_line[index] << " ";

            // Keep going from the next column
            start_index = index + 1;
        }

        // Output till end of the line if not reached already
        if (start_index < in_line.length()) {
            out << in_line.substr(start_index);
        }

        out << std::endl;
    }
}

void forbcc::preprocessor::execute() const {
    // Creates a file in /tmp to be used as support
    std::string middle_filename = make_temp(_in_filename, ".forbpp.middle");

    remove_comments(_in_filename, middle_filename);

    expand_symbols(middle_filename, _out_filename);

    remove(middle_filename);
}