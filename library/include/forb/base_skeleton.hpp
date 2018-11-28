//
// Created by gabriele on 04/11/18.
//

#ifndef LIBFORB_BASE_SKELETON_HPP
#define LIBFORB_BASE_SKELETON_HPP

#include <mutex>
#include <thread>
#include <vector>

#include "forb/stream/socket.hpp"


namespace forb {
    class base_skeleton {
    private:
        using thread_list_t = std::vector<std::thread>;

        int                   port      = 0;
        int                   queuesize = -1;
        forb::streams::socket incoming_calls{};
        std::thread           listen_thread{};

        // The mutex protects the running_calls variable
        std::mutex    calls_mutex{};
        bool          can_accept        = false;
        thread_list_t running_calls{};


    public:
        static void listen_thread_body(base_skeleton *impl);

        static void call_thread_body(base_skeleton *impl,
                                     forb::streams::socket callsocket);

    public:
        explicit base_skeleton(int port) : port(port) {
        };

        base_skeleton(int
                      port, int
                      queuesize)
                : base_skeleton(port) {
            this->queuesize = queuesize;
        };

        // incoming_calls = forb::streams::socket::make_server(port, queuesize);

        virtual ~base_skeleton() {};

    private:
        forb::streams::socket accept_call();

    protected:
        virtual void execute_call(call_id_t code,
                                  forb::streams::stream *callstream,
                                  forb::streams::stream *datastream) = 0;

    public:
        void start_server();

        void stop_server();
    };
}


#endif //LIBFORB_BASE_SKELETON_HPP
