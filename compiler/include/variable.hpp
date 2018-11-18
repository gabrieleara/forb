#ifndef FORBCC_VARIABLE_H
#define FORBCC_VARIABLE_H

#include "entity.hpp"
#include "types/type_custom.hpp"
#include "templates/shareable.hpp"

namespace forbcc {
    class code_ostream;

    /// Represents either an attribute of a forbcc::type_custom class or an automatic variable declared within a
    /// function
    class variable : public entity, public shareable<variable> {
        /// The type associated with the variable.
        std::shared_ptr<const type> _var_type;

    public:
        /* ********************************************** CONSTRUCTORS ********************************************** */

        /// Constructs a variable with a given type.
        /// Variables don't care about the scope, since they are never referred with their fully qualified name in any
        /// context.
        variable(const std::shared_ptr<const type> &var_type, const std::string &name)
                : entity(nullptr, name),
                  _var_type(var_type) {};

        /**************************************************************************************************************/

        /// This class is virtual, so it requires a virtual destructor
        ~variable() override = default;

        /// This class supports moving
        variable(variable &&) = default;

        /// This class supports moving
        variable &operator=(variable &&) = default;

        /// This class supports copying
        variable(const variable &) = default;

        /// This class supports copying
        variable &operator=(const variable &) = default;

        /**************************************************************************************************************/

        const std::shared_ptr<const type> &var_type() const {
            return _var_type;
        };

        /// Prints the variable declaration, proxy to forbcc::type::print_var_declaration, which is a virtual function,
        /// so the actual type of the variable will handle its print.
        void print_declaration(code_ostream &out) const override {
            _var_type->print_var_declaration(out, name());
        };

        /// Does nothing, variables do not require definition.
        void print_definition(code_ostream &out __attribute__((unused))) const override {};

        /// Prints the variable declaration, proxy to forbcc::type::print_var_reference, which is a virtual function,
        /// so the actual type of the variable will handle its print.
        void print_reference(code_ostream &out) const {
            _var_type->print_var_reference(out, name());
        };

        /// Prints the variable declaration, proxy to forbcc::type::print_var_marshal, which is a virtual function,
        /// so the actual type of the variable will handle its print.
        void print_marshal(code_ostream &out, const marshal &do_undo) const {
            _var_type->print_var_marshal(out, name(), do_undo);
        };

        /// Prints the variable declaration, proxy to forbcc::type::print_var_serialize, which is a virtual function,
        /// so the actual type of the variable will handle its print.
        void print_serialize(code_ostream &out, const serialize &do_undo) const {
            _var_type->print_var_serialize(out, name(), do_undo);
        };

    };

} // namespace forbcc

#endif //FORBCC_VARIABLE_H
