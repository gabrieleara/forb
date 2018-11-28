#ifndef FORBCC_TYPE_H
#define FORBCC_TYPE_H

#include "../entity.hpp"
#include "../module.hpp"

namespace forbcc {

    /// Enum specifying which operation between marshalling and unmarshalling is required
    enum class marshal {
        MARSHAL, UNMARSHAL
    };

    /// Enum specifying which operation between serialization and deserialization is required
    enum class serialize {
        SEND, RECV
    };

    /// Base class used to define primitive types, custom types (structures) and arrays
    class type : public entity {
    public:
        /* ********************************************** CONSTRUCTORS ********************************************** */

        /// Empty type, used to preallocate types in arrays or to use later assignment operator
        type() : entity() {};

        /// Using constructors from superclass
        type(const std::shared_ptr<module> &parent, const std::string &name) : entity(parent, name) {};

        /**************************************************************************************************************/

        /// This class is virtual, so it requires a virtual destructor
        ~type() override = default;

        /// This class supports moving
        type(type &&) = default;

        /// This class supports moving
        type &operator=(type &&) = default;

        /// This class supports copying
        type(const type &) = default;

        /// This class supports copying
        type &operator=(const type &) = default;

        /**************************************************************************************************************/

        /// Print the declaration of a variable of this type
        virtual void print_var_declaration(code_ostream &out, const std::string &var_name) const;

        /// Print the declaration of a reference to a variable of this type
        virtual void print_var_reference(code_ostream &out, const std::string &var_name) const;

        /// Print the actions needed to either marshal or unmarshal a variable of this type
        virtual void print_var_marshal(code_ostream &out, const std::string &var_name,
                                       const marshal &do_undo) const;

        /// Print the actions needed to either serialize or deserialize a variable of this type
        virtual void print_var_serialize(code_ostream &out, const std::string &var_name,
                                         const serialize &do_undo) const;
    };

} // namespace forbcc

#endif //FORB_TYPE_H
