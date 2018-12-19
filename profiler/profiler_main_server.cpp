//
// Created by gabriele on 04/12/18.
//

#include <iostream>

#include "profiler_class.hpp"

using namespace forb_profiler;
/*
class profiler_class_impl : public profiler_class_skeleton {

public:
    // Importing constructors from base class
    using profiler_class_skeleton::profiler_class_skeleton;

    // Methods as listed in the specification file

    // Each of them does nothing of course

    int32_t method00(int8_t *) override {
        return 0;
    }

    int32_t method01(int8_t *) override {
        return 0;
    }

    int32_t method02(int8_t *) override {
        return 0;
    }

    int32_t method03(int8_t *) override {
        return 0;
    }

    int32_t method04(int8_t *) override {
        return 0;
    }

    int32_t method05(int8_t *) override {
        return 0;
    }

    int32_t method06(int8_t *) override {
        return 0;
    }
};
*/
class profiler_impl : public profiler_skeleton {

public:
    // Importing constructors from base class
    using profiler_skeleton::profiler_skeleton;

    // The method itself does nothing of course
    int method(int32_t *) override {
        return 0;
    };

    forb_profiler::roba method2(int32_t [1048576]) override {
        return forb_profiler::roba{};
    }
};

int main() {
    profiler_impl server_single{13994};

    server_single.start_server();

    server_single.join_server();
}