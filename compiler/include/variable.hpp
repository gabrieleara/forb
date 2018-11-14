#ifndef FORBCC_VARIABLE_H
#define FORBCC_VARIABLE_H

#include <string> // std::string

namespace forbcc {

    class type;

    class code_ostream;

    class custom_type;

    class variable {
    public:
        const forbcc::custom_type *scope;
        const forbcc::type &var_type;
        const std::string name;

        variable() = delete;
        //variable(variable &) = delete;
        //variable(variable &&) = delete;

        variable(const custom_type *scope, const type &var_type,
                 const std::string name) : scope(scope), var_type(var_type), name(name) {};

        void print_declaration(code_ostream &output) const;

        void print_reference(code_ostream &output) const;

        void print_marshal(code_ostream &output) const;

        void print_unmarshal(code_ostream &output) const;

        void print_recv(code_ostream &output) const;

        void print_send(code_ostream &output) const;
    };

} // namespace forbcc

#endif //FORBCC_VARIABLE_H
