#ifndef FORBCC_VARIABLE_H
#define FORBCC_VARIABLE_H

#include "entity.hpp"
#include "types/type_custom.hpp"

namespace forbcc {
    class code_ostream;

    class variable : public entity {
    public:
        const forbcc::type &var_type;

        variable(const type &var_type, const std::string &name)
                : entity(nullptr, name),
                  var_type(var_type) {};

        void print_declaration(code_ostream &out) const override {
            var_type.print_var_declaration(out, name);
        };

        void print_definition(code_ostream &out __attribute__((unused))) const override {};

        void print_reference(code_ostream &out) const {
            var_type.print_var_reference(out, name);
        };

        void print_marshal(code_ostream &out, const marshal &do_undo) const {
            var_type.print_var_marshal(out, name, do_undo);
        };

        void print_serialize(code_ostream &out, const serialize &do_undo) const {
            var_type.print_var_serialize(out, name, do_undo);
        };

    };

} // namespace forbcc

#endif //FORBCC_VARIABLE_H
