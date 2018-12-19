#ifndef FORBCC_PRIMITIVE_TYPE_H
#define FORBCC_PRIMITIVE_TYPE_H

#include <templates/shareable.hpp>
#include <types/type.hpp>
#include <module.hpp>


namespace forbcc {

    /// Identifies one of the primitive types recognized by the library when parsing a FORB IDL file.
    class type_primitive : public type, public shareable<type_primitive> {

        /* *********************************************** ATTRIBUTES *********************************************** */
    private:
        /// The type that should be printed instead of the name, because to enable compatibility across various
        /// platforms all integers exchanged between components will have fixed size.
        std::string actual_type;

        /// The size of the actual_type
        size_t _size = 0;

        /* ************************************************* STATIC ************************************************* */
    public:
        /// The type of the global list of all known type_primitive pointers.
        using type_primitive_list = ordered_unique_list<ptr_const_t>;

        /// The list of all known primitive types.
        /// Basically it's a dictionary of all known primitive types, no other type may be added to this list outside
        /// than by library maintainers.
        static type_primitive_list known_types;

    private:
        /// Used to initialize statically the list of known types at program startup
        class static_initializer {
        public:
            /// Initializes the list of known types to a complete list of all supported primitive types
            static_initializer();
        };

        /// Used to initialize statically the list of known types at program startup
        static static_initializer _init;

        /* ********************************************** CONSTRUCTORS ********************************************** */
    public:
        /// Empty primitive type, used to preallocate variables in arrays or to use later assignment operator
        type_primitive() : type() {};

        /// Each primitive types corresponds to an actual type with fixed size
        type_primitive(const std::string &name, const std::string &actual_type, const size_t size)
                : type(nullptr, name), actual_type(actual_type), _size(size) {};

        /**************************************************************************************************************/

        /// This class is virtual, so it requires a virtual destructor
        ~type_primitive() override = default;

        /// This class supports moving
        type_primitive(type_primitive &&) = default;

        /// This class supports moving
        type_primitive &operator=(type_primitive &&) = default;

        /// This class supports copying
        type_primitive(const type_primitive &) = default;

        /// This class supports copying
        type_primitive &operator=(const type_primitive &) = default;

        /**************************************************************************************************************/

        /// Does nothing, primitive types do not need to be declared.
        void print_declaration(code_ostream &) const override {};

        /// Does nothing, primitive types do not need to be defined.
        void print_definition(code_ostream &) const override {};

        /// The codename for primitive types is defined by their actual type
        std::string codename() const final {
            return actual_type;
        };

        /// The size of the given type
        size_t size_of() const override {
            return _size;
        };

        /// Returns the alignment of the given type
        size_t alignment() const override {
            return _size;
        };


    };

} // namespace forbcc

#endif //FORBCC_PRIMITIVE_TYPE_H
