#ifndef FORBCC_PRIMITIVE_TYPE_H
#define FORBCC_PRIMITIVE_TYPE_H

#include "type.hpp"
#include "module.hpp"
#include "templates/shareable.hpp"

// TODO: document this
namespace forbcc {
    class type_primitive : public type, public shareable<type_primitive> {
        const std::string actual_type;

    public:
        using type_primitive_list = ordered_unique_list<ptr_const_t>;
        static type_primitive_list known_types;

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

    private:
        class static_initializer {
        public:
            static_initializer();
        };

        static static_initializer _init;
    };

} // namespace forbcc

#endif //FORBCC_PRIMITIVE_TYPE_H
