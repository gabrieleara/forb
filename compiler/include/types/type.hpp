#ifndef FORBCC_TYPE_H
#define FORBCC_TYPE_H

#include <string>

namespace forbcc {
    // Forward declarations to be used within this class declaration
    class code_ostream;

    class variable;

    /**
     * @brief   The base class for types referred within the FORB file, either
     *          primitive or complex
     */
    class type {
    public:
        const std::string name;

        type() = delete;
        //type(type &) = delete;
        //type(type &&) = delete;

        type(const std::string &name) : name(name) {};

        virtual ~type() = 0;

        virtual std::string get_codename() const;

        virtual std::string get_reference_name() const;

        virtual void print_declaration(code_ostream &output) const;

        virtual void print_definition(code_ostream &output, const std::string &scope) const;

        virtual void print_var_declaration(code_ostream &output, const std::string &var_name) const;

        virtual void print_var_reference(code_ostream &output, const std::string &var_name) const;

        virtual void print_var_marshal(code_ostream &output, const std::string &var_name) const;

        virtual void print_var_unmarshal(code_ostream &output, const std::string &var_name) const;

        virtual void print_var_recv(code_ostream &output, const std::string &var_name) const;

        virtual void print_var_send(code_ostream &output, const std::string &var_name) const;
    };

} // namespace forbcc

#endif //FORB_TYPE_H
