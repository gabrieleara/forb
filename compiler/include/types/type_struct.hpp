#ifndef FORBCC_CUSTOM_TYPE_H
#define FORBCC_CUSTOM_TYPE_H

#include <string>
#include <map>

#include <templates/ordered_unique_list.hpp>
#include <templates/shareable.hpp>

#include <types/type.hpp>
#include <module.hpp>
#include <variable.hpp>

namespace forbcc {
    /// Structure types are basically structures defined by the user that can be transferred between library components
    class type_struct : public type, public shareable<type_struct>, public ordered_unique_list<variable> {

        /* ********************************************** CONSTRUCTORS ********************************************** */
    public:
        /// Empty structure type, used to preallocate variables in arrays or to use later assignment operator
        type_struct() : type() {};

        /// Using constructors from superclass
        type_struct(const std::shared_ptr<module> &parent, const std::string &name) : type(parent, name) {};

        /**************************************************************************************************************/

        /// This class is virtual, so it requires a virtual destructor
        ~type_struct() override = default;

        /// This class supports moving
        type_struct(type_struct &&) = default;

        /// This class supports moving
        type_struct &operator=(type_struct &&) = default;

        /// This class supports copying
        type_struct(const type_struct &) = default;

        /// This class supports copying
        type_struct &operator=(const type_struct &) = default;

        /**************************************************************************************************************/

        /// Custom types require a declaration of course
        void print_declaration(code_ostream &out) const override;

        /// Custom types define a partial specialization of the marshal/unmarshal templates
        void print_definition(code_ostream &out) const override;
    };

} // namespace forbcc

#endif //FORBCC_CUSTOM_TYPE_H
