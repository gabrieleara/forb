//
// Created by gabriele on 14/11/18.
//

#ifndef FORB_METHOD_H
#define FORB_METHOD_H

#include <string>
#include <map>

// TODO: avoid reordering of values, but keep track of inserted names
namespace forbcc {
    // Forward declarations

    class code_ostream;

    class type;

    class parameter;

    class method {
        using parameters_list = std::map<std::string, parameter>;
        parameters_list parameters;
        int counters[3] = {0, 0, 0};

    public:
        const type &return_type;
        const std::string name;

    public:
        method(const type &return_type, const std::string &&name) : return_type(return_type), name(name) {};

        void print_declaration(code_ostream &) const;

        void print_stub_definition(code_ostream &output, std::string &&scope, std::string &&method_id) const;

        void print_skeleton_definition(code_ostream &output) const;

        bool insert_parameter(const parameter &param);

    private:
        void print_prototype(forbcc::code_ostream &output, const std::string &thename) const;


    };
}


#endif //FORB_METHOD_H
