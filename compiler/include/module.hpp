//
// Created by gabriele on 14/11/18.
//

#ifndef FORBCC_MODULE_H
#define FORBCC_MODULE_H

#include <string>

#include "entity.hpp"
#include "ordered_unique_list.hpp"

namespace forbcc {
    class module : public entity, public virtual ordered_unique_list<entity*> {

    protected:
        /// If parent module is nullptr, the created
        /// module is equivalent to the global namespace.
        module(const nullptr_t) : entity(nullptr, "") {};

    public:
        /// Static member representing the global namespace
        static module global_module;

        /// Constructs a module within a parent module with a given name,
        // using entity::entity;
        module(const entity *parent, const std::string &name) : entity(parent, name) {};

        void print_declaration(code_ostream &out) const override;

        void print_definition(code_ostream &out) const override;

        /*

        /// Inserts a new entity in the module.
        /// Proxy call to ordered_unique_list<entity>::insert
        bool insert(std::string key, const entity &value) {
            return entities.insert(key, value);
        };

         */
    };


}


#endif //FORBCC_NAMESPACE_H
