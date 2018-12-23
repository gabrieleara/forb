//
// Created by gabriele on 04/12/18.
//

#include <iostream>

#include "profiler_class.hpp"

using namespace forb_profiler;

// NOTICE: last bit must be 1 (or a 1 followed by zeros until the end of the chunk)

int32_t hash(const int32_t *data, size_t length) {
    int32_t res = data[0];

    for (size_t i = 1; i < length; ++i)
        res ^= data[i];

    return res;
}

class profiler_impl : public profiler_skeleton {

public:
    // Importing constructors from base class
    using profiler_skeleton::profiler_skeleton;

    int method0(int data[1048576    ]) override { return hash(data, 1048576    ); };
    int method1(int data[2097152    ]) override { return hash(data, 2097152    ); };
    int method2(int data[4194304    ]) override { return hash(data, 4194304    ); };
    int method3(int data[8388608    ]) override { return hash(data, 8388608    ); };
    int method4(int data[16777216   ]) override { return hash(data, 16777216   ); };
    int method5(int data[33554432   ]) override { return hash(data, 33554432   ); };
    int method6(int data[67108864   ]) override { return hash(data, 67108864   ); };
    int method7(int data[134217728  ]) override { return hash(data, 134217728  ); };


//    int method4(int data[268435456]) override { return hash(data, 268435456); };
//    int method5(int data[1073741824]) override { return hash(data, 1073741824); };
};

int main() {
    profiler_impl server_single{13994};

    server_single.start_server();

    server_single.join_server();
    server_single.join_server();
}