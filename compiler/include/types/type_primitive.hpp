#ifndef FORBCC_PRIMITIVE_TYPE_H
#define FORBCC_PRIMITIVE_TYPE_H

#include "type.hpp"
#include "module.hpp"
#include "templates/shareable.hpp"

namespace forbcc {
    class type_primitive : public type, public shareable<type_primitive> {
        const std::string actual_type;

    public:
        /// Does nothing, primitive types do not need to be declared.
        void print_declaration(code_ostream &out __attribute__((unused))) const override {};

        /// Does nothing, primitive types do not need to be defined.
        void print_definition(code_ostream &out __attribute__((unused))) const override {};

        /// Each primitive types corresponds to an actual type with fixed size
        type_primitive(const std::string &name, const std::string &actual_type)
                : type(nullptr, name), actual_type(actual_type) {};

        /// The codename for primitive types is defined by their actual type
        std::string codename() const final {
            return actual_type;
        };
    };

} // namespace forbcc

#endif //FORBCC_PRIMITIVE_TYPE_H
