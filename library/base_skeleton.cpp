//
// Created by gabriele on 04/11/18.
//

#include <forb/stream/shared_memory.hpp>
#include <forb/base_skeleton.hpp>
#include <forb/exception.hpp>

using stream = forb::streams::stream;
using ssocket = forb::streams::socket;
using shared_memory = forb::streams::shared_memory;

/// The mask that will be used to separate the call ID from the bit used to determine
/// whether the data exchange will be performed over a socket or a shared memory area.
static forb::call_id_t shmem_mask = forb::call_id_t_cast(stream::type::SHMEM);

// For documentation of following methods, see corresponding header file

void forb::base_skeleton::listen_thread_body(forb::base_skeleton *impl) {
    forb::streams::socket call_socket;
    std::thread           conn_thread;

    try {
        while (true) {
            impl->accept_connection();
        }
    } catch (forb::exception &ex) {
        // FIXME: what to do here?
    }
}


void forb::base_skeleton::call_thread_body(forb::base_skeleton *impl, ssocket call_socket) {

    try {
        while (true) {
            // First we receive the id of the call (ORed with a bit that
            // determines whether data exchange will use shared memory area or not).
            forb::call_id_t code;
            call_socket.recv(&code, sizeof(code));

            if (call_socket.require_marshal()) {
                code = forb::streams::unmarshal(code);
            }

            // Separating the information bit from the actual call ID.
            bool use_shmem = (code & shmem_mask) != 0;
            code &= ~shmem_mask;

            // The shared memory that will be used if necessary
            shared_memory shmem_ref;

            // The pointer to the stream that will be actually used (either call_socket or shmem_ref).
            stream *datastream = nullptr;

            if (use_shmem) {
                // The key of the shared memory is given through the call_socket
                shared_memory::key_t key;
                call_socket.recv(&key, sizeof(key));

                if (call_socket.require_marshal()) {
                    key = forb::streams::unmarshal(key);
                }

                shmem_ref = shared_memory::get(key);

                // The datastream will be the shared memory
                datastream = &shmem_ref;
            } else {
                // The datastream will be the socket
                datastream = &call_socket;
            }

            impl->execute_call(code, &call_socket, datastream);
        }
    } catch (forb::exception &ex) {
        // FIXME: what to do here?
    }

    // Destructors will do the cleanup
}

void forb::base_skeleton::accept_connection() {
    ssocket     call_socket = _incoming_calls.accept();
    std::thread conn_thread = std::thread{forb::base_skeleton::call_thread_body,
                                          this,
                                          std::move(call_socket)
    };

    // The thread will keep running independently from the thread object handle
    conn_thread.detach();
}

void forb::base_skeleton::start_server() {
    if (_server_thread.joinable()) {
        throw forb::exception{"Server already started. To start a new instance, please call join_server first."};
    }

    _incoming_calls = forb::streams::socket::make_server(_port, _queue_size);
    _server_thread  = std::thread{&forb::base_skeleton::listen_thread_body, this};
}

void forb::base_skeleton::join_server() {
    if (_server_thread.joinable()) {
        _server_thread.join();
    }

    _incoming_calls.close();
}