#ifndef LIBFORB_SOCKET_HPP
#define LIBFORB_SOCKET_HPP

#include <string>
#include <unistd.h> // close
#include <cstring>  // memset
#include <arpa/inet.h>
#include <sys/socket.h>

#include <cassert>
#include <forb/stream/stream.hpp>

namespace forb {
    namespace streams {

        // Returns true if the machine is big endian
        // (same byte order of network endianess).
        inline bool is_big_endian() {
            union {
                uint32_t i;
                char     c[4];
            } bint = {0x01020304};

            return bint.c[0] == 1;
        }

        /// C++ wrapper of the Socket POSIX API, which uses IPv4 and blocking communication.
        class socket : public virtual stream {
            /* *********************************** ALIASES AND STATIC ATTRIBUTES ************************************ */
        private:
            using sockaddr_in_t = struct sockaddr_in;
            using sockaddr_t = struct sockaddr;

            /// Structure used to indicate that the socket shall be
            /// actually created by the constructor (since there
            /// cannot be two default constructors).
            /// This is actually a technique frequently used within
            /// the standard library too when useless arguments are
            /// needed to overload methods.
            struct socket_create_t {
            };

            // Constants used within the class
            static constexpr int       AF_TYPE       = AF_INET;
            static constexpr int       PROTOCOL      = SOCK_STREAM;
            static constexpr socklen_t SOCKADDR_SIZE = sizeof(sockaddr_in_t);

            /* ********************************************* ATTRIBUTES ********************************************* */
        private:
            /// The file descriptor of the current socket
            int _sock_fd = 0;

            /// The address associated with the current socket, to be used with bind
            std::string _local_address = "";

            /// The port number associated with the current socket, to be used with bind
            int _local_port = 0;

            /// The address of the remote host, if a remote host is connected
            std::string _remote_address = "";

            /// The port number of the remote host, if a remote host is connected
            int _remote_port = 0;

            /// Marks a server socket
            bool _is_server = false;

            /* ******************************************** CONSTRUCTORS ******************************************** */
        private:
            /// Constructs a new socket. Default constructor creates an
            /// non-initialized socket, which can be useful when declaring
            /// socket arrays or when the actual socket will be assigned later.
            /// socket(true) constructor will instead create a new socket,
            /// which can be later binded to a certain address or connected
            /// to a remote host.
            /// NOTICE: the missing "explicit" keyword is intended.
            socket(socket_create_t);

            /// Constructs a new communication socket that is already connected to a
            /// remote host, usually invoked as a result of the accept.
            socket(int sock_fd, const std::string &remote_address, int remote_port);

            /// Constructs a new communication (client) socket, which will be connected to the
            /// remote host specified by the address/port pair.
            socket(const std::string &remote_address, int remote_port);

        public:
            /// Default constructor, creates an empty socket, which cannot be used to exchange data.
            socket() = default;

            /**********************************************************************************************************/

            /// Virtual destructor that closes the socket (if open).
            ~socket() override {
                close();
            };

            /// This class does not support copy construction.
            socket(const socket &other) = delete;

            /// This class does not support copy assignment.
            socket &operator=(const socket &other) = delete;

            /// This class supports move construction.
            /// The moved socket gets reset to the default values for each of its attributes.
            socket(socket &&other) noexcept;

            /// This class supports move assignment.
            /// The moved socket gets reset to the default values for each of its attributes.
            socket &operator=(socket &&other) noexcept;

            /**********************************************************************************************************/

            // Following static methods are implicitly inline

            /// Creates a new server socket.
            /// The new server will be binded to any address on the local machine.
            /// The listen_queue_size argument is optional and defaulted to 10.
            static socket make_server(int port, int listen_queue_size = 10) {
                return make_server("", port, listen_queue_size);
            }

            /// Creates a new server socket.
            /// The new server will be binded to the address specified as argument, or to any address on the local
            /// machine if an empty string is given as argument.
            /// The listen_queue_size argument is optional and defaulted to 10.
            static socket make_server(const std::string &address, int port, int listen_queue_size = 10) {
                socket new_socket = socket_create_t();
                new_socket.bind(address, port);
                new_socket.listen(listen_queue_size);
                return new_socket;
            }

            /// Creates a new client socket, connected to the specified remote host.
            static socket make_client(const std::string &address, int port) {
                socket new_socket = socket_create_t();
                new_socket.connect(address, port);
                return new_socket;
            }

            /**********************************************************************************************************/
        private:
            /// Builds a struct sockaddr_in object starting from the given address
            /// and port number pair.
            static inline sockaddr_in_t build_sockaddr(const std::string &address, int port);

            /// Utility function used to perform move construction and move assignment operations
            void _move_attributes(socket &other) noexcept;

            /*
            /// Binds a socket to the given port number and to any address on the local machine.
            void bind(int port) {
                return bind("", port);
            };
             */

            /// Binds a socket to the given address and port number pair.
            /// If address argument is equal to an empty string, then any address associated with
            /// the machine will be binded to this socket.
            void bind(const std::string &address, int port);

            /// Marks the socket as a server socket.
            void listen(int listen_queue_size);

            /// Connects to the remote host identified by the given address and port number pair.
            void connect(const std::string &address, int port);

        public:
            /// Performs the accept operation on the given server socket. Socket must be a server socket (i.e. a socket
            /// that executed a bind and a listen before).
            socket accept();

            /// Blocking wrapper of the POSIX send function.
            /// It repeats the underlying call until all data are sent or an error occurs.
            void send(const void *buffer, std::size_t size) override;

            /// Blocking wrapper of the POSIX recv function.
            /// It repeats the underlying call until all data are received or an error occurs.
            void recv(void *buffer, std::size_t size) override;

            /// Closes the socket, called by the virtual destructor.
            void close() noexcept override;

            /// Returns the type of the stream.
            type get_type() const override {
                return type::SOCKET;
            };

            /// Returns true if the execution platform requires marshalling before sending
            /// data on the socket, false otherwise.
            /// Sending over a network requires marshalling (byteswapping) if the machine is not
            /// big endian.
            bool require_marshal() const override {
                return !is_big_endian();
            };
        };

    } // namespace streams

} // namespace forb

#endif // #ifndef LIBFORB_SOCKET_HPP
