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

    /// Represents the parameter of a forbcc::method, each associated with a direction between either input, output or
    /// both.
    class parameter {
        /// The direction of the parameter
        direction _direction;

        /// The underlying local variable used to interact with the parameter
        variable _variable;

    public:
        /* ********************************************** CONSTRUCTORS ********************************************** */

        /// Constructs a parameter
        parameter(const direction &dir, const variable &var)
                : _direction(dir),
                  _variable(var) {};

        /**************************************************************************************************************/

        /// This class is not virtual, so it does not require a virtual destructor
        ~parameter() = default;

        /// This class supports moving
        parameter(parameter &&) = default;

        /// This class supports moving
        parameter &operator=(parameter &&) = default;

        /// This class supports copying
        parameter(const parameter &) = default;

        /// This class supports copying
        parameter &operator=(const parameter &) = default;

        /**************************************************************************************************************/

        /// Returns the direction of the parameter.
        direction dir() const {
            return _direction;
        };

        /// Returns a reference to the variable used to express the parameter.
        const variable &var() const {
            return _variable;
        };

        /// Prints the declaration, which may change whether the parameter is IN or OUT/INOUT.
        /// Basically parameters which are used as output are passed by reference.
        /// NOTICE that also arrays are always passed by reference and they may be modified
        /// even if they are only input parameters, depending whether the adopted stream requires
        /// marshalling or not. Never use an array after passing it to a method declared in this way.
        void print_declaration(code_ostream &out) const {
            if (_direction == direction::IN) {
                // Pass by value
                _variable.print_declaration(out);
            } else {
                // Pass by reference
                _variable.print_reference(out);
            }
        };


    };

}


#endif //FORBCC_PARAMETER_H
