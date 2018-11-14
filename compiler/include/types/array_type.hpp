#ifndef FORBCC_ARRAY_TYPE_H
#define FORBCC_ARRAY_TYPE_H

#include <string>
#include "type.hpp"

namespace forbcc {

    class array_type : public type {
    public:
        const type &item_type;
        const int length;

        array_type() = delete;

        //array_type(array_type &) = delete;
        array_type(array_type &&) = delete;

        // Any trivially copiable type can be accepted as array item type
        // We limit that to only types known by the library though
        array_type(const type &item_type, const int length)
                : type("forb_array_" + item_type.name),
                  item_type(item_type),
                  length(length) {};

        std::string get_codename() const override;       // item_type.get_actual_type() << "[" << length << "]";
        std::string get_reference_name() const override; // get_codename()

        void print_var_declaration(code_ostream &output, const std::string &var_name) const override;
        // oss << item_type.get_actual_type() << " " << name << "[" << length << "]" << ";";

        void print_var_reference(code_ostream &output, const std::string &var_name) const override;

        void print_var_marshal(code_ostream &output, const std::string &var_name) const override;

        void print_var_unmarshal(code_ostream &output, const std::string &var_name) const override;

        void print_var_recv(code_ostream &output, const std::string &var_name) const override;

        void print_var_send(code_ostream &output, const std::string &var_name) const override;

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
