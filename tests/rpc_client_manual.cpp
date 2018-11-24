//
// Created by gabriele on 02/11/18.
//

#include <iostream>

#include "rpc_class_manual.hpp"

using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Please, give as argument the name of the file to be opened for the Remote Registry." << endl;
        return EXIT_FAILURE;
    }

    forb::remote_registry registry{argv[1]};

    example::rpc_class_var myobj = example::rpc_class::_assign(registry.get("rpc_obj"));
    // example::rpc_class_ptr myobj = example::rpc_class::_narrow(registry.get("rpc_obj"));

    int life = myobj->sum_ints(37, 5);

    cout << "The meaning of life is " << life << "!" << endl;

    int magic = myobj->sum_ints(1, 2);

    cout << "But the magic number is " << magic << " of course!" << endl;

    return EXIT_SUCCESS;
}

