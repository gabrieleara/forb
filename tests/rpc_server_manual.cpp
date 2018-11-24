//
// Created by gabriele on 02/11/18.
//

#include "rpc_class_manual.hpp"

int main() {
    example::rpc_class_impl server{13994};

    server.start_server();

    while (true) {}
}

