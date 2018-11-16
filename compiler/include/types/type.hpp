#ifndef FORBCC_TYPE_H
#define FORBCC_TYPE_H

#include "../entity.hpp"
#include "../module.hpp"

namespace forbcc {
    enum class marshal { MARSHAL, UNMARSHAL };
    enum class serialize { SEND, RECV };

    // Base class used to define primitive types, custom types (structures) and arrays
    class type : public entity {
    public:
        /// Including constructors from superclass
        // using entity::entity;
        type(const module *parent, std::string name) : entity(parent, name) {};

        /// Implementing entity method
        void print_declaration(code_ostream &out) const override;

        /// Implementing entity method
        void print_definition(code_ostream &out) const override;

        /// Print the declaration of a variable of this type
        virtual void print_var_declaration(code_ostream &out, const std::string &var_name) const;

        /// Print the declaration of a reference to a variable of this type
        virtual void print_var_reference(code_ostream &out, const std::string &var_name) const;

        /// Print the actions needed to either marshal or unmarshal a variable of this type
        virtual void print_var_marshal(code_ostream &out, const std::string &var_name, const marshal do_undo) const;

        /// Print the actions needed to either serialize or deserialize a variable of this type
        virtual void print_var_serialize(code_ostream &out, const std::string &var_name, const serialize do_undo) const;
    };

} // namespace forbcc

#endif //FORB_TYPE_H
