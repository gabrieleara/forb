//
// Created by gabriele on 02/11/18.
//

#include <iostream>

#include "remote_registry.hpp"
#include "rpc_class_manual_example.hpp"

using namespace std;

int main() {
    forb::remote_registry registry{"rpc.json"};

    examples::rpc_class_var myobj = examples::rpc_class::_assign(registry.get("rpc_obj"));
    // examples::rpc_class_ptr myobj = examples::rpc_class::_narrow(registry.get("rpc_obj"));

    int life = myobj->sum_ints(37, 5);

    cout << "The meaning of life is " << life << "!" << endl;

    int magic = myobj->sum_ints(1, 2);

    cout << "But the magic number is " << magic << " of course!" << endl;

    return 0;
}

