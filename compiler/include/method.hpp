//
// Created by gabriele on 14/11/18.
//

#ifndef FORBCC_METHOD_H
#define FORBCC_METHOD_H

#include "entity.hpp"
#include "ordered_unique_list.hpp"

#include "interface.hpp"
#include "parameter.hpp"

namespace forbcc {
    // Forward declarations
    class code_ostream;

    class type;

    class method : public entity, public virtual ordered_unique_list<parameter> {
        int counters[3] = {0, 0, 0};

    public:
        const type &return_type;

    public:
        method(const interface *parent, const std::string &name, const type &return_type)
                : entity(parent, name),
                  return_type(return_type) {};

        void print_declaration(code_ostream &out) const override;

        void print_definition(code_ostream &out __attribute__((unused))) const override {

        };

        void print_virtual_declaration(code_ostream &out) const;

        void print_stub_definition(code_ostream &out, const std::string &scope, const std::string &enumname) const;

        void print_skeleton_definition(code_ostream &out) const;

        bool insert(std::string key, const parameter &param) override {
            bool success = ordered_unique_list::insert(key, param);

            if (success) {
                ++counters[static_cast<int>(param.dir)];
            }

            return success;
        };

        std::string id() const;

    private:
        void print_prototype(forbcc::code_ostream &out, const std::string &thename) const;
    };
}


#endif //FORBCC_METHOD_H
