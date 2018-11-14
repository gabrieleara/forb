//
// Created by gabriele on 04/11/18.
//

#ifndef LIBFORB_BASE_STUB_HPP
#define LIBFORB_BASE_STUB_HPP

#include <vector>
#include <string>
#include <memory>

#include "stream/shared_memory.hpp"
#include "stream/socket.hpp"
#include "stream/stream.hpp"

namespace forb {
    // NOTICE: should only be used for integer types

    // TODO: thread safety using a mutex to avoid concurrent calls

    // Forward declaration of registry class
    class remote_registry;

    class base_stub {
        friend class remote_registry;

    protected:
        using ssocket = forb::streams::socket;
        using shared_memory = forb::streams::shared_memory;
        using stream = forb::streams::stream;

    private:
        ssocket       _socket{};
        shared_memory _shmem{};

    public:
        // Rule of the five
        base_stub(const base_stub& other) = delete;
        base_stub(base_stub&& other) = default;

    protected:
        stream *rpcstream;
        stream *datastream;

        virtual shared_memory::size_t get_buffer_size() { return 1024; };

    protected:
        base_stub() = default;

    public:
        // This way any stub class can have the destructor it wants
        virtual ~base_stub() noexcept {};

    private:
        static bool is_local_address(const std::string &ip);

    private:
        void set_skeleton(std::string ip, int port) {
            _socket = ssocket::make_client(ip, port);

            rpcstream = &_socket;

            bool is_local = is_local_address(ip);

            if (is_local) {
                _shmem     = shared_memory::create(get_buffer_size());
                datastream = &_shmem;
            } else {
                datastream = rpcstream;
            }
        };

    protected:
        void init_call(call_id_t code);

        void wait_return();

    protected:
        virtual bool _match(const std::string &type) const = 0;

        virtual base_stub *_create_empty() const = 0;

        static std::vector<base_stub *> &_protos();

        static base_stub *_create(const std::string &type);

    };

    /* ********************** TEMPLATED UTILITY TYPES *********************** */

    using remote_ptr = std::shared_ptr<base_stub>;
    using remote_var = std::unique_ptr<base_stub>;

    /* ******************** TEMPLATED UTILITY FUNCTIONS ********************* */

    template<typename T>
    bool is_nil(const std::shared_ptr<T> &ptr) {
        static_assert(std::is_base_of<base_stub, T>::value,
                      "The given type is not derived from forb::base_stub!");
        return ptr == nullptr;
    }

    template<typename T>
    bool is_nil(const std::unique_ptr<T> &ptr) {
        static_assert(std::is_base_of<base_stub, T>::value,
                      "The given type is not derived from forb::base_stub!");
        return ptr == nullptr;
    }


} // namespace forb


#endif //LIBFORB_BASE_STUB_HPP
