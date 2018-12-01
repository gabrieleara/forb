//
// Created by gabriele on 19/11/18.
//

#ifndef FORBCC_PARSER_H
#define FORBCC_PARSER_H

#include <string>

namespace forbcc {

    /// Enumerator that is used to distinguish between which FORB IDL keyword has been matched.
    /// See forbcc::keywords.
    enum class keywords_idx_t {
        MODULE, STRUCT, INTERFACE, NUM_KEYWORDS
    };

    /// The set of keywords used in FORB IDL language.
    extern const std::string keywords[];

    /// The set of keywords used to specify parameters direction in FORB IDL language.
    extern const std::string direction_keywords[];

    /// The set of reserved names by FORB IDL language
    extern const std::string reserved_names[];

    /// The length of forbcc::reserved_names array
    extern const int reserved_names_length;

    /// Parses an already pre-processed FORB IDL file and populates forbcc::module::global_module with
    /// all the parsed data.
    class parser {
        std::string filename;

    public:
        /// Creates a new parser with the given input file.
        explicit parser(const std::string &filename) : filename(filename) {};

        /// Performs the parse of the given file and populates forbcc::module::global_module with
        /// all the parsed data.
        void execute();
    };

}


#endif //FORBCC_PARSER_H
