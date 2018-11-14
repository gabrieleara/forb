//
// Created by gabriele on 02/11/18.
//

#include "rpc_class_manual_example.hpp"

int main() {
    examples::rpc_class_impl server{13994};

    server.start_server();

    while(true) {}
}

