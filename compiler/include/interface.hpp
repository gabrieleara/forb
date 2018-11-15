//
// Created by gabriele on 15/11/18.
//

#ifndef FORBCC_INTERFACE_H
#define FORBCC_INTERFACE_H

#include <string>
#include <map>

namespace forbcc {
    class method;

    class code_ostream;

    class interface {
        const std::string name;

        using methods_list = std::map<std::string, method>;
        methods_list methods;

    public:
        explicit interface(const std::string &&name) : name(name) {};

        bool insert_method(const method &themethod);

        void generate_declaration(code_ostream &output) const;

        void generate_definition(code_ostream &output) const;

    private:

        void generate_stub_declaration(code_ostream &output) const;

        void generate_skeleton_declaration(code_ostream &output) const;

        void generate_methods_enum(code_ostream &output) const;

        void generate_static_attributes_definition(code_ostream &output) const;

        void generate_factory_match(code_ostream &output) const;

        void generate_factory_create(code_ostream &output) const;

        void generate_narrows(code_ostream &output) const;

        void generate_stub_methods(code_ostream &output) const;

        void generate_skeleton_method(code_ostream &output) const;

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

        std::string codename() const {
            // TODO: add modules support
            return name;
        }
    };
} // namespace forbcc


#endif //FORB_INTERFACE_H
