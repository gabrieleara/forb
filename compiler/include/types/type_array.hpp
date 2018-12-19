#ifndef FORBCC_ARRAY_TYPE_H
#define FORBCC_ARRAY_TYPE_H

#include <string>
#include <algorithm>
#include <cassert>

#include <types/type.hpp>
#include <templates/shareable.hpp>

namespace forbcc {

    /// Identifies an array of a given type, to be used in forbcc::method or in forbcc::type_struct definitions.
    class type_array : public type, public shareable<type_array> {

        /* ************************************************* ALIAS ************************************************** */
    public:
        /// The type of the length attribute (which is a signed type, but values less than zero are considered invalid)
        using length_t = long;

        /* *********************************************** ATTRIBUTES *********************************************** */
    private:
        /// The type of the elements of the array, can be any trivially copyable type.
        std::shared_ptr<const type> _item_type;

        /// The length of the array
        length_t _length = 0;

        /// The result of a sizeof on the array
        size_t _size = 0;

        /* ************************************************* STATIC ************************************************* */
    public:
        /// The type of the global list of all known type_array pointers.
        using type_array_list = ordered_unique_list<std::shared_ptr<const type_array>>;

        /// The list of all known arrays.
        /// Basically it's a dictionary of all known array types, to avoid re-declaration of already-defined types.
        /// Each type is characterized both by its item type and its length.
        static type_array_list arrays;

        /// Converts a pair of item_type and length to an unique identifier, to be used when storing known array types.
        static std::string to_identifier(const std::shared_ptr<const type> &item_type, const length_t length) {
            // FIXME: This could probably lead to conflicts, but they would be extremely rare.
            std::string codename = item_type->codename();
            std::replace(codename.begin(), codename.end(), ':', '_');
            return "_Forb_Array_" + item_type->codename() + "_D_" + std::to_string(length) + "_";
        };

        /* ********************************************** CONSTRUCTORS ********************************************** */
    public:
        /// Empty array type, used to preallocate variables in arrays or to use later assignment operator
        type_array() : type(), _item_type(nullptr) {};

        /// Any trivially copyable type can be accepted as an array item type (except another array_type), while
        /// the length of the array must be a strictly positive integer.
        /// NOTICE that no checking is done to ensure that item_type is not an array.
        type_array(const std::shared_ptr<const type> &item_type, const length_t length)
                : type(nullptr, to_identifier(item_type, length)),
                  _item_type(item_type),
                  _length(length),
                  _size(item_type->size_of() * length) {
            assert((length > 0 && "The length of the array must be strictly positive!"));
        };

        /// Arrays of arrays cannot be declared
        type_array(const type_array *item_type, const int length) = delete;

        /**************************************************************************************************************/

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
        void print_declaration(code_ostream &) const override {};

        /// Does nothing, primitive types do not need to be defined.
        void print_definition(code_ostream &) const override {};

        /// Returns the type of the items of the given array.
        std::shared_ptr<const type> item_type() const {
            return _item_type;
        };

        /// Returns the length of the array
        length_t length() const {
            return _length;
        };

        /// Prints the declaration of a variable of the given type (no semicolon).
        void print_var_declaration(code_ostream &out, const std::string &var_name) const override;

        /// Print the definition of a variable of this type
        void print_var_definition(code_ostream &out, const std::string &var_name, bool force_stack) const override;

        /// Print the acquisition of the value of a variable of this type
        void print_var_lvalue(code_ostream &out, const std::string &var_name, bool is_stack) const override;

        /// Prints the declaration of a reference variable of the given type (no semicolon).
        void print_var_reference(code_ostream &out, const std::string &var_name) const override;

        /// Prints the marshalling/unmarshalling operation of a variable of the given type.
        void print_var_marshal(code_ostream &out, const std::string &var_name, const marshal &do_undo,
                               bool force_stack) const override;

        /// Prints the serialization/deserialization of a variable of the given type.
        void print_var_serialize(code_ostream &out, const std::string &var_name, const serialize &do_undo,
                                 bool force_stack) const override;

        /// Returns the size of the given type
        size_t size_of() const override {
            return _size;
        };

        /// Returns the alignment of the given type
        size_t alignment() const override {
            return item_type()->alignment();
        };
    };

} // namespace forbcc

#endif //FORBCC_ARRAY_TYPE_H
