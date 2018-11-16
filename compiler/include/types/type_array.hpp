#ifndef FORBCC_ARRAY_TYPE_H
#define FORBCC_ARRAY_TYPE_H

#include <string>
#include "type.hpp"

namespace forbcc {

    class type_array : public type {
    public:
        const type &item_type;
        const int length;

        /// Any trivially copiable type can be accepted as an array item type
        type_array(const type &item_type, const int length)
                : type(nullptr, "forb_array_" + item_type.name),
                  item_type(item_type),
                  length(length) {};

        /// Arrays of arrays cannot be declared
        type_array(const type_array &item_type, const int length) = delete;

        void print_var_declaration(code_ostream &output, const std::string &var_name) const override;

        void print_var_reference(code_ostream &output, const std::string &var_name) const override;

        void print_var_marshal(code_ostream &out, const std::string &var_name, const marshal do_undo)
        const override;

        void print_var_serialize(code_ostream &out, const std::string &var_name, const serialize do_undo)
        const override;

        // protected:
        // virtual bool should_getter_return_const() const { return false; };

        /*
            virtual void print_var_getter_prototype(code_ostream &output, const variable &,
                                                    bool fullyqualified = false) const override;

            virtual void print_var_setter_prototype(code_ostream &output, const variable &,
                                                    bool fullyqualified = false) const override;

            virtual void print_var_getter_body(code_ostream &output, const variable &) const override;

            virtual void print_var_setter_body(code_ostream &output, const variable &) const override;
        */
    };

} // namespace forbcc

#endif //FORBCC_ARRAY_TYPE_H
