//
// Created by gabriele on 04/11/18.
//

#include <stream/shared_memory.hpp>
#include "base_skeleton.hpp"

using base_skeleton = forb::base_skeleton;

using stream = forb::streams::stream;
using ssocket = forb::streams::socket;
using shared_memory = forb::streams::shared_memory;

void base_skeleton::listen_thread_body(base_skeleton *impl) {
    bool should_continue = true;
    {
        std::lock_guard<std::mutex> lock{impl->calls_mutex};
        should_continue = impl->can_accept;
    }

    try {
        while (should_continue) {
            forb::streams::socket callsocket = impl->accept_call();

            std::lock_guard<std::mutex> lock{impl->calls_mutex};
            if (impl->can_accept) {
                std::thread t{base_skeleton::call_thread_body, impl, std::move(callsocket)};
                impl->running_calls.push_back(std::move(t));
            } else {
                should_continue = false;
            }
        }
    } catch (int e) {
    }
}


constexpr forb::call_id_t shmem_mask = forb::call_id_t_cast(stream::type::SHMEM);

void base_skeleton::call_thread_body(base_skeleton *impl, ssocket callsocket) {

    try {
        while (true) {// TODO: change
            forb::call_id_t code;
            callsocket.recv(&code, sizeof(code));

            if (callsocket.require_marshal()) {
                code = forb::streams::unmarshal(code);
            }

            // This will tell whether shared memory shall be used, then we need to filter
            // out the type of the call
            bool use_shmem = (code & shmem_mask) != 0;
            code &= ~shmem_mask;

            shared_memory shmem_ref;

            stream *datastream = nullptr;

            if (use_shmem) {
                shared_memory::key_t key;
                callsocket.recv(&key, sizeof(key));

                if (callsocket.require_marshal()) {
                    key = forb::streams::unmarshal(key);
                }

                shmem_ref  = shared_memory::get(key);
                datastream = &shmem_ref;
            } else {
                datastream = &callsocket;
            }

            impl->execute_call(code, &callsocket, datastream);
        }
    } catch (int e) {
    }

    // Destructors will do the cleanup

    // TODO: Should also remove the thread from the vector
}

forb::streams::socket base_skeleton::accept_call() {
    return incoming_calls.accept();
}

void base_skeleton::start_server() {
    // TODO: should check whether the server is already running

    can_accept     = true;
    incoming_calls = forb::streams::socket::make_server(port, queuesize);
    listen_thread  = std::thread{&base_skeleton::listen_thread_body, this};
}

// TODO: actually call threads never terminate, thus another mechanism is needed to terminate the program
void base_skeleton::stop_server() {
    {
        std::lock_guard<std::mutex> lock{calls_mutex};
        can_accept = false;
    }

    for (auto &it : running_calls) {
        it.join();
    }

    // Remove all threads from the vector
    running_calls.clear();

    {
        std::lock_guard<std::mutex> lock{calls_mutex};
        // Destroy forcefully the listening thread and close the listening socket
        listen_thread  = std::thread{};
        incoming_calls = forb::streams::socket{};
    }
}