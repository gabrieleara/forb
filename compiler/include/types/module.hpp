//
// Created by gabriele on 14/11/18.
//

#ifndef FORBCC_MODULE_H
#define FORBCC_MODULE_H

#include <string>

namespace forbcc {
    class module {

    public:
        const module* parent;
        const std::string name;

        module(const module *parent) : parent(parent) {};

        std::string get_codename() const;
    };

    extern module global_module;
}


#endif //FORBCC_NAMESPACE_H
