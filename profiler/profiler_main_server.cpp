//
// Created by gabriele on 04/12/18.
//

#include <iostream>

#include "profiler_class.hpp"

using namespace forb_profiler;

/// Calculates the checksum of the given data.
int32_t hash(const int32_t *data, size_t length) {
    int32_t res = data[0];

    for (size_t i = 1; i < length; ++i)
        res ^= data[i];

    return res;
}

/// Implementation of the profiler_skeleton class, which simply calls the hash function
/// every time it receives some data.
class profiler_impl : public profiler_skeleton {
public:
    /// Importing constructors from base class
    using profiler_skeleton::profiler_skeleton;

    /// Calculates the checksum of 4MB of data
    int method0(int data[1048576]) override { return hash(data, 1048576); };

    /// Calculates the checksum of 8MB of data
    int method1(int data[2097152]) override { return hash(data, 2097152); };

    /// Calculates the checksum of 16MB of data
    int method2(int data[4194304]) override { return hash(data, 4194304); };

    /// Calculates the checksum of 32MB of data
    int method3(int data[8388608]) override { return hash(data, 8388608); };

    /// Calculates the checksum of 64MB of data
    int method4(int data[16777216]) override { return hash(data, 16777216); };

    /// Calculates the checksum of 128MB of data
    int method5(int data[33554432]) override { return hash(data, 33554432); };

    /// Calculates the checksum of 256MB of data
    int method6(int data[67108864]) override { return hash(data, 67108864); };

    /// Calculates the checksum of 512MB of data
    int method7(int data[134217728]) override { return hash(data, 134217728); };
};

int main() {
    profiler_impl server_single{13994};

    server_single.start_server();

    // The server never ends
    server_single.join_server();
}