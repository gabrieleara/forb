//
// Created by gabriele on 02/11/18.
//

#include "example.hpp"

#include <cstring>

class rpc_class_impl : public example::rpc_class_skeleton {

public:
    // Importing constructors from base class
    using rpc_class_skeleton::rpc_class_skeleton;

    // Methods as listed in the specification file
    int sum(int a, int b) override;
};

int rpc_class_impl::sum(int a, int b) {
    return a + b;
}


int main() {
    rpc_class_impl server{13994};

    server.start_server();

    server.join_server();
}

