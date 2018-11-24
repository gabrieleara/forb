//
// Created by gabriele on 19/11/18.
//

#ifndef FORBCC_PARSER_H
#define FORBCC_PARSER_H

#include <string>

namespace forbcc {

    enum class keywords_idx_t {
        MODULE, TYPE, INTERFACE, IN, OUT, INOUT, NUM_KEYWORDS
    };

    extern const std::string keywords[static_cast<int>(keywords_idx_t::NUM_KEYWORDS)];

    class parser {
        std::string filename;

    public:

        explicit parser(const std::string &filename) : filename(filename) {};

        void execute();
    };

}


#endif //FORBCC_PARSER_H
