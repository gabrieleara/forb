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

    /// Custom types are basically structures defined by the user
    class type_custom : public type, public virtual ordered_unique_list<variable> {
        /// Ordered list of attributes of the custom type
        // ordered_unique_list<variable> attributes;

    public:
        /// Using constructors from superclass
        type_custom(const module *parent, std::string name) : type(parent, name) {};

        /// Custom types require a declaration of course
        void print_declaration(code_ostream &out) const override;

        /// Custom types define a partial specialization of the marshal/unmarshal templates
        void print_definition(code_ostream &out) const override;

        /*
        /// Inserts a new attribute in the module.
        /// Proxy call to ordered_unique_list<variable>::insert
        bool insert(std::string key, const variable &attr) {
            return attributes.insert(key, attr);
        };
         */
    };

} // namespace forbcc

#endif //FORBCC_CUSTOM_TYPE_H
