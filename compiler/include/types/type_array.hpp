#ifndef FORBCC_ARRAY_TYPE_H
#define FORBCC_ARRAY_TYPE_H

#include <string>
#include <algorithm>
#include "type.hpp"
#include "templates/shareable.hpp"

namespace forbcc {

    /// Identifies an array of a given type, to be used in forbcc::method or in forbcc::type_custom definitions.
    class type_array : public type, public shareable<type_array> {
    public:
        /// The type of the elements of the array, can be any trivially copyable type.
        std::shared_ptr<const type> _item_type;

        /// The length of the array
        size_t _length;

        /// A list of array pointers
        using type_array_list = ordered_unique_list<std::shared_ptr<const type_array>>;

        /// The list of all known arrays
        static type_array_list arrays;

        /* ********************************************** CONSTRUCTORS ********************************************** */

        /// Any trivially copyable type can be accepted as an array item type
        type_array(const std::shared_ptr<const type> &item_type, const size_t length)
                : type(nullptr, to_identifier(item_type, length)),
                  _item_type(item_type),
                  _length(length) {};

        /// Arrays of arrays cannot be declared
        type_array(const type_array *item_type, const int length) = delete;

        /**************************************************************************************************************/

        static std::string to_identifier(const std::shared_ptr<const type> &item_type, const size_t length) {
            // TODO: could this lead to conflitcs?
            std::string codename = item_type->codename();
            std::replace(codename.begin(), codename.end(), ':', '_');
            return "_Forb_Array_" + item_type->codename() + "_D_" + std::to_string(length) + "_";
        }

        /// This class is virtual, so it requires a virtual destructor
        ~type_array() override = default;

        /// This class supports moving
        type_array(type_array &&) = default;

        /// This class supports moving
        type_array &operator=(type_array &&) = default;

        /// This class supports copying
        type_array(const type_array &) = default;

        /// This class supports copying
        type_array &operator=(const type_array &) = default;

        /**************************************************************************************************************/

        /// Does nothing, primitive types do not need to be declared.
        void print_declaration(code_ostream &out __attribute__((unused))) const override {};

        /// Does nothing, primitive types do not need to be defined.
        void print_definition(code_ostream &out __attribute__((unused))) const override {};

        /// Returns the type of the items of the given array.
        std::shared_ptr<const type> item_type() const {
            return _item_type;
        }

        /// Returns the length of the array
        size_t length() const {
            return _length;
        };

        /// Prints the declaration of a variable of the given type (no semicolon).
        void print_var_declaration(code_ostream &output, const std::string &var_name) const override;

        /// Prints the declaration of a reference variable of the given type (no semicolon).
        void print_var_reference(code_ostream &output, const std::string &var_name) const override;

        /// Prints the marshalling/unmarshalling operation of a variable of the given type.
        void print_var_marshal(code_ostream &out, const std::string &var_name, const marshal &do_undo)
        const override;

        /// Prints the serialization/deserialization of a variable of the given type.
        void print_var_serialize(code_ostream &out, const std::string &var_name, const serialize &do_undo)
        const override;
    };

} // namespace forbcc

#endif //FORBCC_ARRAY_TYPE_H
