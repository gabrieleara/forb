//
// Created by gabriele on 15/11/18.
//

#ifndef FORBCC_INTERFACE_H
#define FORBCC_INTERFACE_H

#include "entity.hpp"
#include "module.hpp"
#include "ordered_unique_list.hpp"

namespace forbcc {
    class method;
    class module;

    class code_ostream;

    class interface : public entity, public virtual ordered_unique_list<method>{
    public:
        interface(const module *parent, const std::string &name) : entity(parent, name) {};

        void print_declaration(code_ostream &out) const override;

        void print_definition(code_ostream &out) const override;

    private:

        void print_stub_declaration(code_ostream &output) const;

        void print_skeleton_declaration(code_ostream &output) const;

        void print_methods_enum(code_ostream &output) const;

        void print_static_attributes_definition(code_ostream &output) const;

        void print_factory_match(code_ostream &output) const;

        void print_factory_create(code_ostream &output) const;

        void print_narrows(code_ostream &output) const;

        void print_stub_methods(code_ostream &output) const;

        void print_skeleton_method(code_ostream &output) const;

        inline std::string name_ptr() const {
            return name + "_ptr";
        };

        inline std::string name_var() const {
            return name + "_var";
        };

        inline std::string name_skeleton() const {
            return name + "_skeleton";
        };

        inline std::string name_enum() const {
            return name + "_method_codes";
        };

        inline std::string codename_ptr() const {
            return codename() + "_ptr";
        };

        inline std::string codename_var() const {
            return codename() + "_var";
        };

        inline std::string codename_skeleton() const {
            return codename() + "_skeleton";
        };
    };
} // namespace forbcc


#endif //FORB_INTERFACE_H
