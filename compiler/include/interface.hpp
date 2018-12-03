//
// Created by gabriele on 15/11/18.
//

#ifndef FORBCC_INTERFACE_H
#define FORBCC_INTERFACE_H

#include "entity.hpp"
#include "module.hpp"
#include "templates/ordered_unique_list.hpp"
#include "templates/shareable.hpp"

namespace forbcc {
    // Forward declarations
    class method;

    class code_ostream;

    /// Defines an interface, which will be converted to stub and skeleton pairs upon generation
    class interface : public entity, public shareable<interface>, public ordered_unique_list<method> {
    public:

        /* ********************************************** CONSTRUCTORS ********************************************** */

        /// Empty interface, used to preallocate variables in arrays or to use later assignment operator
        interface() : entity() {};

        /// Using constructors from superclass
        interface(const std::shared_ptr<module> &parent, const std::string &name) : entity(parent, name) {};

        /**************************************************************************************************************/

        /// This class is virtual, so it requires a virtual destructor
        ~interface() override = default;

        /// This class supports moving
        interface(interface &&) = default;

        /// This class supports moving
        interface &operator=(interface &&) = default;

        /// This class supports copying
        interface(const interface &) = default;

        /// This class supports copying
        interface &operator=(const interface &) = default;

        /**************************************************************************************************************/

        /// Prints both stub and skeletons declarations
        void print_declaration(code_ostream &out) const override;

        /// Prints both stub and skeletons definitions, among other utilities
        void print_definition(code_ostream &out) const override;

    private:

        /// Prints stub class declaration
        void print_stub_declaration(code_ostream &out) const;

        /// Prints skeleton class declaration
        void print_skeleton_declaration(code_ostream &out) const;

        /// Prints the enum class used to identify methods called
        void print_methods_enum(code_ostream &out) const;

        /// Prints the definition of static attributes needed by the factory pattern
        void print_static_attributes_definition(code_ostream &out) const;

        /// Prints match function definition needed by the factory pattern
        void print_factory_match(code_ostream &out) const;

        /// Prints create function definition needed by the factory pattern
        void print_factory_create(code_ostream &out) const;

        /// Prints narrow and assign functions definitions needed to downcast pointers to base stubs to pointers to the
        /// given interface's stub
        void print_narrows(code_ostream &out) const;

        /// Prints stub methods
        void print_stub_methods(code_ostream &out) const;

        /// Prints skeleton call dispatch method
        void print_skeleton_method(code_ostream &out) const;

        /// Returns the name of the stub class pointer type
        std::string name_ptr() const {
            return name() + "_ptr";
        };

        /// Returns the name of the stub class variable type
        std::string name_var() const {
            return name() + "_var";
        };

        /// Returns the name of the skeleton class
        std::string name_skeleton() const {
            return name() + "_skeleton";
        };

        /// Returns the name of the enum class used to dispatch method calls
        std::string name_enum() const {
            return name() + "_method_codes";
        };

        /// Returns the fully qualified name of the stub class pointer type
        std::string codename_ptr() const {
            return codename() + "_ptr";
        };

        /// Returns the fully qualified name of the stub class variable type
        std::string codename_var() const {
            return codename() + "_var";
        };

        /// Returns the fully qualified name of the skeleton class
        std::string codename_skeleton() const {
            return codename() + "_skeleton";
        };
    };
} // namespace forbcc


#endif //FORBCC_INTERFACE_H
