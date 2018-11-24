//
// Created by gabriele on 14/11/18.
//

#ifndef FORBCC_METHOD_H
#define FORBCC_METHOD_H

#include "entity.hpp"
#include "templates/ordered_unique_list.hpp"

#include "interface.hpp"
#include "parameter.hpp"

namespace forbcc {
    // Forward declarations
    class code_ostream;

    /// Defines a method of a forbcc::interface, which can have an arbitrary number of input, output or inout
    /// parameters.
    class method : public entity, public ordered_unique_list<parameter> {
        /// Counts the number of input, output and inout parameters
        int _counters[3] = {0, 0, 0};

        /// Reference to the return type of the method
        std::shared_ptr<const type> _return_type;

    public:
        /* ********************************************** CONSTRUCTORS ********************************************** */

        /// Constructs a new method for the given parent interface with a name and a return type
        method(const std::shared_ptr<interface> &parent, const std::string &name,
               const std::shared_ptr<const type> &return_type)
                : entity(parent, name),
                  _return_type(return_type) {};

        /**************************************************************************************************************/

        /// This class is virtual, so it requires a virtual destructor
        ~method() override = default;

        /// This class supports moving
        method(method &&) = default;

        /// This class supports moving
        method &operator=(method &&) = default;

        /// This class supports copying
        method(const method &) = default;

        /// This class supports copying
        method &operator=(const method &) = default;

        /**************************************************************************************************************/

        /// Prints the method declaration
        void print_declaration(code_ostream &out) const override;

        /// THIS FUNCTION DOES NOTHING, use the print_stub_definition and print_skeleton_definition methods respectively
        /// according to requirements!
        /// The function was put only because this class is a subclass of the forbcc::entity class, which has a pure
        /// virtual print_definition method.
        void print_definition(code_ostream &out __attribute__((unused))) const override {};

        /// Prints the virtual declaration of a method
        void print_virtual_declaration(code_ostream &out) const;

        /// Prints the definition of the method of the stub
        void print_stub_definition(code_ostream &out, const std::string &scope, const std::string &enumname) const;

        /// Prints the definition of the skeleton code wrapping the virtual call to the method, defined by subclasses
        /// of the generated skeleton class.
        void print_skeleton_definition(code_ostream &out) const;

        /// Proxy to ordered_unique_list<parameter> that counts the number of parameters of each type.
        bool insert(std::string key, const parameter &param) override {
            bool success = ordered_unique_list::insert(key, param);

            if (success) {
                ++_counters[static_cast<int>(param.dir())];
            }

            return success;
        };

        /// Returns the method id, which will be stored within an enum class defined for each interface.
        /// Method id is obtained as a combination of the method name and its paramethers, so that methods overloading
        /// is possible for remote objects.
        std::string id() const;

    private:
        /// Method called by print_declaration, print_virtual_declaration and print_stub_definition.
        /// It is used to have the same basic structure for each method, but in a way that methods can be adapted to be
        /// declared/defined either within class declaration (no full scope qualification needed) or outside (full
        /// scope qualification needed).
        void print_prototype(forbcc::code_ostream &out, const std::string &thename) const;
    };
}


#endif //FORBCC_METHOD_H
