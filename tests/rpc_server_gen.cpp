//
// Created by gabriele on 02/11/18.
//

#include "rpc_class_generated.hpp"

#include <cstring>

class rpc_class_impl : public example::rpc_class_skeleton {

public:
    // Importing constructors from base class
    using rpc_class_skeleton::rpc_class_skeleton;

    // Methods as listed in the specification file
    int sum(int a, int b, example::structure &c) override;
};

int rpc_class_impl::sum(int a, int b, example::structure &c) {
    std::string result = "Hello World!";
    strncpy(c.a3, result.c_str(), result.length()+1);
    c.a1 = a*2;
    c.a2 = b*3;

    return a + b;
}


int main() {
    rpc_class_impl server{13994};

    server.start_server();

    while (true) {}
}

