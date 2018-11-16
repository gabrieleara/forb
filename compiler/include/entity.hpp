#include <utility>

//
// Created by gabriele on 16/11/18.
//

#ifndef FORBCC_ENTITY_H
#define FORBCC_ENTITY_H

#include <string>

namespace forbcc {
    class code_ostream;

    class entity {
    protected:
        entity(nullptr_t) : parent(nullptr), name("") {};

    public:
        const entity *parent;
        const std::string name;

        entity(const entity *parent, std::string name) : parent(parent), name(std::move(name)) {};

        virtual void print_declaration(code_ostream &out) const = 0;

        virtual void print_definition(code_ostream &out) const = 0;

        virtual std::string codename() const {
            std::string parentname = (parent != nullptr) ? parent->codename() : "";

            return (parentname.length()) ? parentname + "::" + name : name;
        };
    };
}


#endif //FORBCC_ENTITY_H
