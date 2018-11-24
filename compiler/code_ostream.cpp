//
// Created by gabriele on 14/11/18.
//

#include "code_ostream.hpp"

// For documentation, see corresponding header file

void forbcc::code_ostream::code_stringbuf::putOutput() {
    // Notice: this function is called also by the destructor.
    std::string content = str();

    // Print indentation if the line is not empty
    // BUG: known bug, it prints indentation even if the content is an empty line.
    if (content.length() > 0) {
        // NOTE: Could be improved by using the fill constructor of the std::string class
        for (int i = 0; i < indentation; ++i) {
            output << "    "; // Each indentation is four spaces
        }
    }

    // Print buffer
    output << content;

    // Reset buffer
    str("");

    // Flush output
    output.flush();
}

