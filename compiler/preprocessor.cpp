//
// Created by gabriele on 20/11/18.
//

#include "preprocessor.hpp"

#include "file_utils.hpp"
#include "exception.hpp"

#include <string>
#include <fstream>
#include <sstream>

#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <cassert>

// For documentation of forbcc::preprocessor class, see corresponding header file

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
                throw forbcc::exception{"parse of input file",
                                        "unexpected symbol at line "
                                        + std::to_string(line_num)
                                        + " column "
                                        + std::to_string(index)
                                        + ": '/'"};
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
                    throw forbcc::exception{"parse of input file",
                                            "unexpected symbol at line "
                                            + std::to_string(line_num)
                                            + " column "
                                            + std::to_string(index)
                                            + ": '/'"};
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
        throw forbcc::exception{"parse of input file", "unexpected EOF, expected end of multi-line comment via \"*/\""};
    }
}

/// Places a space before and after each symbol used in FORB IDL language.
/// Used to simplify the compiler's job to identify names of elements.
inline void expand_symbols(const std::string &in_filename, const std::string &out_filename) {

    // Input and output streams
    std::ifstream in{in_filename};
    std::ofstream out{out_filename};

    // Recognized symbols in FORB IDL language.
    const std::string symbols = "{}[]();,";

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

    assert(!middle_filename.empty() && "middle_filename argument cannot be empty!");

    remove_comments(_in_filename, middle_filename);

    expand_symbols(middle_filename, _out_filename);

    remove(middle_filename);
}