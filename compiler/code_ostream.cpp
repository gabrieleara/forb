//
// Created by gabriele on 14/11/18.
//

#include "code_ostream.hpp"

void forbcc::code_ostream::code_stringbuf::putOutput() {
    // Called by destructor.
    // destructor can not call virtual methods.

    std::string content = str();

    // Print indentation if the line is not empty (TODO: check if this works)
    if (content.length() > 0) {
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

