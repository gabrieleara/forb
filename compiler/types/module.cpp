//
// Created by gabriele on 14/11/18.
//

#include <types/module.hpp>

#include "types/module.hpp"

forbcc::module forbcc::global_module{nullptr};

std::string forbcc::module::get_codename() const {
    if (parent != nullptr)
        return parent->get_codename() + "::" + name;

    return name;
}
