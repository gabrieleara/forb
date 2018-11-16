//
// Created by gabriele on 14/11/18.
//

#ifndef FORBCC_PARAMETER_H
#define FORBCC_PARAMETER_H

#include "variable.hpp"

namespace forbcc {
    class code_ostream;

    enum class direction {
        IN, OUT, INOUT
    };

    class parameter {
    public:
        const direction dir;
        const variable var;

        parameter(const direction dir, const type &var_type, const std::string &name)
                : dir(dir),
                  var(var_type, name) {};

        /// Prints the declaration, may change if the parameter is IN or OUT/INOUT.
        void print_declaration(code_ostream &out) const {
            if (dir == direction::IN) {
                // Pass by value
                var.print_declaration(out);
            } else {
                // Pass by reference
                var.print_reference(out);
            }
        };


    };

}


#endif //FORBCC_PARAMETER_H
