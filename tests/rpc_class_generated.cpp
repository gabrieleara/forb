//
// Created by gabriele on 16/11/18.
//

#include "rpc_class_generated.hpp"
#include <forb/forb.hpp>

using ssocket = forb::streams::socket;
using shared_memory = forb::streams::shared_memory;
using stream = forb::streams::stream;

namespace forb {
    namespace streams {
        template <>
        example::structure marshal<example::structure>(example::structure v) {
            v.a1 = forb::streams::marshal(v.a1);
            v.a2 = forb::streams::marshal(v.a2);
            forb::streams::marshal(v.a3, 10);
            return v;
        }
    }
}

/// Enumerator used to distinguish the requested call.
enum class rpc_class_method_codes : forb::call_id_t {
    _FsumE3int1a3int1b9structure1c,
};

/// Initializing static attributes for example::rpc_class class factory.
example::rpc_class             example::rpc_class::_factory{};
example::rpc_class::init_class example::rpc_class::_init{};

/// Match method, needed by the factory.
bool example::rpc_class::_match(const std::string &type) const {
    return type == "example::rpc_class";
}

/// Create method, used by the factory.
forb::base_stub *example::rpc_class::_create_empty() const {
    return new example::rpc_class{};
}

/// Narrows a generic forb::remote_var reference to a example::rpc_class_ptr if possible.
/// Returns nullptr if not possible.
example::rpc_class_ptr example::rpc_class::_narrow(forb::remote_var &&reference) {
    rpc_class *ptr = dynamic_cast<rpc_class *>(reference.get());

    if (ptr == nullptr) {
        return example::rpc_class_ptr{nullptr};
    } else {
        // The reference is moved to the new one, after correct casting
        return std::dynamic_pointer_cast<rpc_class>(
                forb::remote_ptr{std::move(reference)});
    }
}

/// Narrows a generic forb::remote_ptr reference to a example::rpc_class_ptr if possible.
/// Returns nullptr if not possible.
example::rpc_class_ptr example::rpc_class::_narrow(forb::remote_ptr &reference) {
    rpc_class *ptr = dynamic_cast<rpc_class *>(reference.get());

    if (ptr == nullptr) {
        return example::rpc_class_ptr{nullptr};
    } else {
        // The reference is copied to the new one, after correct casting
        return std::dynamic_pointer_cast<rpc_class>(reference);
    }
}

/// Assigns a generic forb::remote_var reference to a example::rpc_class_var if possible.
/// Returns nullptr if not possible.
example::rpc_class_var example::rpc_class::_assign(forb::remote_var &&reference) {
    rpc_class *ptr = dynamic_cast<rpc_class *>(reference.get());

    if (ptr == nullptr) {
        return rpc_class_var{nullptr};
    } else {
        // The original reference must be manually released first
        reference.release();

        return rpc_class_var{ptr};
    }
}

int example::rpc_class::sum(int a, int b, example::structure &c) {
    this->init_call(forb::call_id_t_cast(rpc_class_method_codes::_FsumE3int1a3int1b9structure1c));

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

    // Read output parameters
    datastream->recv(&c, sizeof(c));

    if (datastream->require_marshal()) {
        c = forb::streams::unmarshal(c);
    }

    // Finally read result
    int resvalue;

    datastream->recv(&resvalue, sizeof(resvalue));

    if (datastream->require_marshal()) {
        resvalue = forb::streams::unmarshal(resvalue);
    }

    return resvalue;
}


/// This method dispatches the call to the right virtual method, which must be redefined in a subclass.
void example::rpc_class_skeleton::execute_call(forb::call_id_t code,
                                               forb::streams::stream *callstream,
                                               forb::streams::stream *datastream) {
    // callstream is the stream used by the RPC protocol and
    // datastream is the one used to exchange actual data
    // They might be the same stream
    switch((rpc_class_method_codes) code) {
        case rpc_class_method_codes::_FsumE3int1a3int1b9structure1c:
            int a;
            int b;
            example::structure c;

            datastream->recv(&a, sizeof(a));
            datastream->recv(&b, sizeof(b));

            if (datastream->require_marshal()) {
                a = forb::streams::unmarshal(a);
                b = forb::streams::unmarshal(b);
            }

            // Send over callstream an ACK
            uint16_t rescode = 1;
            if (callstream->require_marshal()) {
                rescode = forb::streams::marshal(rescode);
            }

            callstream->send(&rescode, sizeof(rescode));

            // Perform virtual call
            int resvalue = sum(a, b, c);

            // Send back output variables
            if (datastream->require_marshal()) {
                c = forb::streams::marshal(c);
            }

            datastream->send(&c, sizeof(c));

            if (datastream->require_marshal()) {
                resvalue = forb::streams::marshal(resvalue);
            }
            datastream->send(&resvalue, sizeof(resvalue));
            break;
    }
}

