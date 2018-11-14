#ifndef __forb_socket_hpp__
#define __forb_socket_hpp__

#include <string>
#include <unistd.h> // close
#include <cstring>  // memset
#include <arpa/inet.h>
#include <sys/socket.h>

#include "stream.hpp"

namespace forb {
    namespace streams {

    // Returns true if the machine is big endian
    // (same byte order of network endianess)
    inline bool is_big_endian(void) {
        union {
            uint32_t i;
            char     c[4];
        } bint = {0x01020304};

        return bint.c[0] == 1;
    }

    class socket : public virtual stream {
        using sockaddr_in_t = struct sockaddr_in;
        using sockaddr_t = struct sockaddr;

        static constexpr int       AF_TYPE       = AF_INET;
        static constexpr int       PROTOCOL      = SOCK_STREAM;
        static constexpr socklen_t SOCKADDR_SIZE = sizeof(sockaddr_in_t);

        int sock_fd;

        // Following data is used only when binding address/port to the socket
        std::string local_address = "";
        int         local_port    = 0;

        // If the socket is used to communicate with a remote host, its data will be
        // put here
        std::string remote_address = "";
        int         remote_port    = 0;

    public:
        socket(bool create = false) {
            if (create) {
                sock_fd = ::socket(AF_TYPE, PROTOCOL, 0);
            } else {
                sock_fd = 0;
            }

            if (sock_fd < 0)
                throw 18;
        };

        socket(const std::string &remote_address, int remote_port)
                : socket(true) {
            this->remote_address = remote_address;
            this->remote_port    = remote_port;
        }

    private:
        socket(const int sock_fd,
               const std::string &remote_address,
               const int remote_port) {
            this->sock_fd        = sock_fd;
            this->remote_address = remote_address;
            this->remote_port    = remote_port;

            // Checking if the socket was created correctly
            if (sock_fd < 0)
                throw 18;
        };

    private:
        inline sockaddr_in_t build_sockaddr(const std::string &address, const int port) {
            sockaddr_in_t address_structure;

            address_structure.sin_family = AF_TYPE;
            address_structure.sin_port   = htons(port);

            if (address.length() < 1) {
                // We don't care about which address, only the port
                address_structure.sin_addr.s_addr = INADDR_ANY;
            } else {
                // We convert the text-based address in a network address
                if (::inet_pton(AF_TYPE, address.c_str(), &address_structure.sin_addr.s_addr) == 0)
                    throw 10;
            }

            return address_structure;
        }

    private:
        void bind(int port) {
            return bind("", port);
        };

        void bind(const std::string &address, const int port) {
            sockaddr_in_t local_address_structure;

            local_address_structure = build_sockaddr(address, port);

            // Force reusage of already binded ports
            int option = 1;
            setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

            int res = ::bind(sock_fd, reinterpret_cast<sockaddr_t *>(&local_address_structure), SOCKADDR_SIZE);

            if (res != 0)
                throw 17;

            local_address = address;
            local_port    = port;
        };

        void listen(int listen_queue_size = 10) {
            int res = ::listen(sock_fd, listen_queue_size);

            if (res != 0)
                throw 42;
        };

        void connect(const std::string &address, const int port) {
            sockaddr_in_t address_structure = build_sockaddr(address, port);

            int res = ::connect(sock_fd,
                                reinterpret_cast<sockaddr_t *>(&address_structure), SOCKADDR_SIZE);

            // std::cout << "Ma qui ci arrivo?" <<std::endl;

            remote_address = address;
            remote_port    = port;

            if (res != 0)
                throw (42);
        };

        void _move_attributes(socket &other) noexcept {
            this->sock_fd        = other.sock_fd;
            this->local_address  = other.local_address;
            this->local_port     = other.local_port;
            this->remote_address = other.remote_address;
            this->remote_port    = other.remote_port;

            other.sock_fd        = 0;
            other.local_address  = "";
            other.local_port     = 0;
            other.remote_address = "";
            other.remote_port    = 0;

            // Remember to move other members if necessary
        };

    public:
        // I delete copy constructor/assignment
        socket(const socket &other) = delete;

        socket &operator=(const socket &other) = delete;

        // But define move constructor/assignment
        socket(socket &&other) noexcept {
            _move_attributes(other);
        };

        socket &operator=(socket &&other) noexcept {
            if (this != &other) // Probably useless self-assignment check
            {
                // If the same memory space is reused,
                // the old socket must be closed
                close();

                _move_attributes(other);
            }

            return *this;
        };

        virtual ~socket() override {
            close();
        }

        static socket make_socket() {
            return socket{true};
        }

        static socket make_server(int port, int listen_queue_size = 10) {
            return make_server("", port, listen_queue_size);
        }

        static socket make_server(const std::string &address, const int port, const int listen_queue_size = 10) {
            socket new_socket{true};

            new_socket.bind(address, port);
            new_socket.listen(listen_queue_size);
            return new_socket;
        }

        static socket make_client(const std::string &address, const int port) {
            socket new_socket{true};
            new_socket.connect(address, port);
            return new_socket;
        }

        // TODO: should check whether this is a server socket or a client socket
        socket accept() {
            sockaddr_in_t remote_address;
            socklen_t     sockaddr_size_var = SOCKADDR_SIZE;

            std::memset(&remote_address, 0, SOCKADDR_SIZE);

            int newsock_fd = ::accept(sock_fd,
                                      reinterpret_cast<struct sockaddr *>(&remote_address),
                                      &sockaddr_size_var);

            if (newsock_fd < 0)
                throw (17);

            char remote_addr_cstr[16];

            auto res = ::inet_ntop(AF_TYPE,
                                   &remote_address,
                                   remote_addr_cstr,
                                   sockaddr_size_var);

            if (res == 0)
                throw (42);

            return socket{newsock_fd, std::string{remote_addr_cstr}, remote_port};
        };

        // NOTICE: blocking
        virtual void send(const void *buffer, std::size_t size) override {
            auto    buffer_ptr    = reinterpret_cast<const unsigned char *>(buffer);
            size_t  remaining     = size;
            ssize_t how_many_sent = 0;

            while (remaining > 0 &&
                   (how_many_sent = ::send(sock_fd,
                                           buffer_ptr,
                                           remaining,
                                           0)) > 0) {
                if (size_t(how_many_sent) > remaining)
                    throw (12); // Underflow

                remaining -= how_many_sent;
                buffer_ptr += how_many_sent;
            }

            if (remaining != 0)
                throw (42);
        };

        // NOTICE: blocking
        virtual void recv(void *buffer, std::size_t size) override {
            // auto    buffer_ptr    = reinterpret_cast<std::byte *>(buffer); // C++17
            auto    buffer_ptr    = reinterpret_cast<unsigned char *>(buffer); // C++98
            size_t  remaining     = size;
            ssize_t how_many_recv = 0;

            while (remaining > 0 &&
                   (how_many_recv = ::read(sock_fd,
                                           buffer_ptr,
                                           remaining)) > 0) {
                if (size_t(how_many_recv) > remaining)
                    throw (12); // Underflow
                remaining -= how_many_recv;
                buffer_ptr += how_many_recv;
            }

            if (remaining != 0)
                throw (42);
        };

        virtual void close() override {
            if (sock_fd > 0) {
                ::close(sock_fd);
                sock_fd = 0;
            }
        };

        type get_type() override {
            return type::SOCKET;
        };

        // Sending over a network requires byteswap if the machine is not
        // big endian
        virtual bool require_marshal() { return !is_big_endian(); };
    };

    } // namespace streams

} // namespace forb

#endif // #ifndef __forb_socket_hpp__
