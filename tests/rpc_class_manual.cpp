//
// Created by gabriele on 02/11/18.
//

// #include <byteswap.h>
// #include <string>
// #include <cstddef>
// #include <memory>

// NOTICE: this is used for testing the library before installation, to use the library after installation please use
// #include <forb/forb.hpp>
#include <forb/forb.hpp> // "forb.hpp"
#include "rpc_class_manual.hpp"

using ssocket = forb::streams::socket;
using shared_memory = forb::streams::shared_memory;
using stream = forb::streams::stream;



/* ***************************** LOCAL_CLASSES ****************************** */

enum class rpc_class_method_codes : forb::call_id_t {
    SUM_INTS = 1,
};

/* ******************************* RPC_CLASS ******************************** */

// Initialize static attributes for factory
example::rpc_class             example::rpc_class::_factory{};
example::rpc_class::init_class example::rpc_class::_init{};


// Custom methods - automatically generated
int example::rpc_class::sum_ints(int a, int b) {
    std::lock_guard<std::mutex> __mutex_guard{base_stub::_mutex};

    this->init_call(forb::call_id_t_cast(rpc_class_method_codes::SUM_INTS));

    // Objects serialization
    if (datastream->require_marshal()) {
        // Data requires marshalling before being sent
        a = forb::streams::marshal(a);
        b = forb::streams::marshal(b);
    }

    // Now write stuff to the stream
    datastream->send(&a, sizeof(a));
    datastream->send(&b, sizeof(b));

    // Wait for the response
    this->wait_return();

    // Read the result / output parameters
    int __resvalue;

    datastream->recv(&__resvalue, sizeof(__resvalue));
    if (datastream->require_marshal()) {
        __resvalue = forb::streams::unmarshal(__resvalue);
    }

    // Return result
    return __resvalue;
}


// Match method, needed by the factory
bool example::rpc_class::_match(const std::string &type) const {
    return type == "example::rpc_class";
}

// Factory method
forb::base_stub *example::rpc_class::_create_empty() const {
    return new rpc_class{};
}


// Static methods used to narrow pointers
example::rpc_class_ptr example::rpc_class::_narrow(forb::remote_var &&reference) {
    rpc_class *ptr = dynamic_cast<rpc_class *>(reference.get());

    if (ptr == nullptr) {
        return rpc_class_ptr{nullptr};
    } else {
        // I need to duplicate the object
        return std::dynamic_pointer_cast<rpc_class>(
                forb::remote_ptr{std::move(reference)});
    }
}

example::rpc_class_ptr example::rpc_class::_narrow(forb::remote_ptr &reference) {
    rpc_class *ptr = dynamic_cast<rpc_class *>(reference.get());

    if (ptr == nullptr) {
        return rpc_class_ptr{nullptr};
    } else {
        // I need to duplicate the object
        return std::dynamic_pointer_cast<rpc_class>(reference);
    }
}

example::rpc_class_var example::rpc_class::_assign(forb::remote_var &&reference) {
    rpc_class *ptr = dynamic_cast<rpc_class *>(reference.get());

    if (ptr == nullptr) {
        return rpc_class_var{nullptr};
    } else {
        // I need to release the original object
        reference.release();

        return rpc_class_var{ptr};
    }
}


/* *************************** RPC_CLASS_SKELETON *************************** */

void example::rpc_class_skeleton::execute_call(forb::call_id_t code,
                                                forb::streams::stream *callstream,
                                                forb::streams::stream *datastream) {
    // callstream is the stream used by the RPC protocol and
    // datastream is the one used to exchange actual data
    // They might be the same stream

    switch ((rpc_class_method_codes) code) {
        case rpc_class_method_codes::SUM_INTS:
            int a;
            int b;
            datastream->recv(&a, sizeof(a));
            datastream->recv(&b, sizeof(b));

            if (datastream->require_marshal()) {
                a = forb::streams::unmarshal(a);
                b = forb::streams::unmarshal(b);
            }

            // Perform virtual call
            int __resvalue = sum_ints(a, b);

            // Send over callsocket an ACK
            // TODO: DO I NEED SPECIFIC CODES? RIGHT NOW IT'S UNUSED
            uint16_t rescode = 1;
            if (callstream->require_marshal()) {
                rescode = forb::streams::marshal(rescode);
            }
            callstream->send(&rescode, sizeof(rescode));

            // Finally send back result
            if (datastream->require_marshal()) {
                __resvalue = forb::streams::marshal(__resvalue);
            }

            datastream->send(&__resvalue, sizeof(__resvalue));
            break;
    }
}

/* ***************************** RPC_CLASS_IMPL ***************************** */

// The following class is NOT automatically generated of course

int example::rpc_class_impl::sum_ints(int a, int b) {
    return a + b;
}