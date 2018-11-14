//
// Created by gabriele on 14/11/18.
//

#ifndef FORBCC_PARAMETER_H
#define FORBCC_PARAMETER_H

#include <string>
#include "code_ostream.hpp"
#include "variable.hpp"
#include "types/array_type.hpp"

namespace forbcc {
    enum class direction {
        IN, OUT, INOUT
    };

    class parameter {
    public:
        const direction dir;
        const variable var;

        parameter(const direction dir, const type &var_type, const std::string name)
                : dir(dir),
                  var(nullptr, var_type, name) {};

        friend code_ostream &operator<<(code_ostream &output, const parameter &param) {
            if (param.dir == direction::IN) {
                // Pass by value
                param.var.print_declaration(output);
            } else {
                // Pass by reference
                param.var.print_reference(output);
            }

            return output;
        }


    };

}


#endif //FORBCC_PARAMETER_H
