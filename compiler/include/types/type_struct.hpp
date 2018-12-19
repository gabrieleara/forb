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
    private:
        /// The size of the structure, increases for each attribute insertion
        size_t _size = 0;

        /// The alignment of the given type, corresponding to larger
        /// alignment amond its attributes
        size_t _alignment = 0;

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

        // Print the definition of a variable of this type
        // void print_var_definition(code_ostream &out, const std::string &var_name, bool force_stack) const override;

        // Print the acquisition of the value of a variable of this type
        // void print_var_lvalue(code_ostream &out, const std::string &var_name, bool is_stack) const override;

        /// Wraps odered_unique_list::insert method adding a few operations to calculate structure size and alignment
        bool insert(std::string key, const variable &value) override;

        /// Returns the size of the given type
        size_t size_of() const override {
            return _size;
        };

        /// Returns the alignment of the given type
        size_t alignment() const override {
            return _alignment;
        };


    };

} // namespace forbcc

#endif //FORBCC_CUSTOM_TYPE_H
