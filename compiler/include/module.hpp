//
// Created by gabriele on 14/11/18.
//

#ifndef FORBCC_MODULE_H
#define FORBCC_MODULE_H

#include <string>

#include <entity.hpp>
#include <templates/ordered_unique_list.hpp>
#include <templates/shareable.hpp>

namespace forbcc {

    // Forward declaration
    class type_struct;

    /// Represents a module, which basically is a namespace, so it's a collection of other entities, which can be either
    /// other modules, custom types or interfaces
    class module : public entity, public shareable<module>, public ordered_unique_list<std::shared_ptr<entity>> {

        /* ************************************************* STATIC ************************************************* */
    public:
        /// Static member representing the global namespace, used as base for the tree-like structure of entities.
        static const std::shared_ptr<module> global_module;

        /* ********************************************** CONSTRUCTORS ********************************************** */
    public:
        /// Empty module, used to preallocate variables in arrays or to use later assignment operator.
        /// It is also the constructor used to allocate the global_module.
        module() : entity() {};

        /// Using constructor from superclass
        module(const std::shared_ptr<entity> &parent, const std::string &name) : entity(parent, name) {};

        /**************************************************************************************************************/
    public:
        /// This class is virtual, so it requires a virtual destructor
        ~module() override = default;

        /// This class supports moving
        module(module &&) = default;

        /// This class supports moving
        module &operator=(module &&) = default;

        /// This class supports copying
        module(const module &) = default;

        /// This class supports copying
        module &operator=(const module &) = default;

        /**************************************************************************************************************/

        /// Prints module declaration (should be called within a header file)
        void print_declaration(code_ostream &out) const override;

        /// Prints module definition (should be called within a source file)
        void print_definition(code_ostream &out) const override;

        /// Returns a pointer to a module identified by name, but only if said module is defined directly within
        /// current module, nullptr otherwise.
        std::shared_ptr<module> get_module(const std::string &name);

        /// Finds a module by its name (which may contain :: qualifiers), following C++ scope resolution and visibility
        /// rules.
        /// The name may be the simple name of a module declared within this module or it may be
        /// the name of a module visible from this module (an ancestor of this module).
        /// Returns a pointer to the requested module if it exists, nullptr otherwise.
        std::shared_ptr<module> find_module(const std::string &name);

        /// Finds a custom structure type by its name (which may contain :: qualifiers), following C++ scope
        /// resolution and visibility rules.
        /// The name may be the simple name of a structure declared within this module or it may be
        /// the name of a structure visible from this module (defined in an upper module).
        /// Returns a pointer to the requested module if it exists, nullptr otherwise.
        std::shared_ptr<type_struct> find_struct(const std::string &name);

    private:
        /// Finds an entity whose name corresponds to the given one, following C++ scope resolution and visibility
        /// rules.
        /// The name may be the simple name of a type visible from within this module or it may be
        /// the name of a type visible from this module (defined in an upper module).
        std::shared_ptr<entity> find(const std::string &name);

    };


}


#endif //FORBCC_NAMESPACE_H
