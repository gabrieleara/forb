//
// Created by gabriele on 14/11/18.
//

#ifndef FORBCC_MODULE_H
#define FORBCC_MODULE_H

#include <string>

#include "entity.hpp"
#include "ordered_unique_list.hpp"

namespace forbcc {

    /// Represents a module, which basically is a namespace, so it's a collection of other entities, which can be either
    /// other modules, custom types or interfaces
    class module : public entity, public ordered_unique_list<std::shared_ptr<entity>> {

    public:
        /// Static member representing the global namespace
        static const std::shared_ptr<module> global_module;

        /* ********************************************** CONSTRUCTORS ********************************************** */

        /// Using constructor from superclass
        module(const std::shared_ptr<const entity> &parent, const std::string &name) : entity(parent, name) {};

    // protected:
        /// If parent module is nullptr, the created module is equivalent to the global namespace.
        /// However, this should only be called to actually create the global namespace, defined as a static property
        /// of this class.
        explicit module(nullptr_t) : entity(nullptr) {};

    public:
        /**************************************************************************************************************/

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
    };


}


#endif //FORBCC_NAMESPACE_H
