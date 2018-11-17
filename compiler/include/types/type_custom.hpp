#ifndef FORBCC_CUSTOM_TYPE_H
#define FORBCC_CUSTOM_TYPE_H

#include <string>
#include <map>

#include "type.hpp"
#include "../module.hpp"
#include "../ordered_unique_list.hpp"

namespace forbcc {
    /// Forward declarations to be used within this class declaration
    class variable;

    /// Custom types are basically structures defined by the user that can be transferred using the library
    class type_custom : public type, public ordered_unique_list<variable> {
    public:
        /* ********************************************** CONSTRUCTORS ********************************************** */

        /// Using constructors from superclass
        type_custom(const std::shared_ptr<const module> &parent, std::string name) : type(parent, name) {};

        /**************************************************************************************************************/

        /// This class is virtual, so it requires a virtual destructor
        ~type_custom() override = default;

        /// This class supports moving
        type_custom(type_custom &&) = default;

        /// This class supports moving
        type_custom &operator=(type_custom &&) = default;

        /// This class supports copying
        type_custom(const type_custom &) = default;

        /// This class supports copying
        type_custom &operator=(const type_custom &) = default;

        /**************************************************************************************************************/

        /// Custom types require a declaration of course
        void print_declaration(code_ostream &out) const override;

        /// Custom types define a partial specialization of the marshal/unmarshal templates
        void print_definition(code_ostream &out) const override;
    };

} // namespace forbcc

#endif //FORBCC_CUSTOM_TYPE_H
