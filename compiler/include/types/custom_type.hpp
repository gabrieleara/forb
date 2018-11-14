#ifndef FORBCC_CUSTOM_TYPE_H
#define FORBCC_CUSTOM_TYPE_H

#include <string>
#include <map>

#include "type.hpp"

namespace forbcc {
    // Forward declarations to be used within this class declaration
    class variable;

    class module;

    /**
     * @brief   The base class for types referred within the FORB file, either
     *          primitive or complex
     */
    class custom_type : public type {
        using attributes_list = std::map<std::string, variable>;

        attributes_list attributes;
        const module &themodule;

    public:
        custom_type() = delete;
        //custom_type(custom_type &) = delete;
        //custom_type(custom_type &&) = delete;

        custom_type(const module &themodule, const std::string &name) : type(name), themodule(themodule) {};

        std::string get_codename() const override;

        void print_declaration(code_ostream &) const override;

        void print_definition(code_ostream &, const std::string &scope) const override;

        void print_var_marshal(code_ostream &output, const std::string &var_name) const override;

        void print_var_unmarshal(code_ostream &output, const std::string &var_name) const override;

        bool insert_attribute(const variable &attr);

    protected:
        // virtual inline bool should_getter_return_const() const override { return true; };
    };

} // namespace forbcc

#endif //FORBCC_CUSTOM_TYPE_H
