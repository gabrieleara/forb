//
// Created by gabriele on 03/12/18.
//

#include <forb/stream/socket.hpp>
#include <forb/exception.hpp>

// For documentation see corresponding header file

/* ************************************************ INLINE FUNCTIONS ************************************************ */

inline forb::streams::socket::sockaddr_in_t forb::streams::socket::build_sockaddr(
        const std::string &address,
        int port) {

    sockaddr_in_t address_structure;

    address_structure.sin_family = AF_TYPE;
    address_structure.sin_port   = htons(port);

    if (address.length() < 1) {
        // Don't care about which address, only the port
        address_structure.sin_addr.s_addr = INADDR_ANY;
    } else {
        // Convert the text-based address in a network address
        int res = ::inet_pton(AF_TYPE, address.c_str(), &address_structure.sin_addr.s_addr);
        if (res == 0) {
            throw forb::exception{"The specified address is not valid."};
        }

        assert(res > 0 && "The AF_TYPE given as input to inet_pton is invalid");
    }

    return address_structure;
}

inline void forb::streams::socket::close() noexcept {
    if (_sock_fd > 0) {
        ::close(_sock_fd);
        _sock_fd = 0;
    }
}

inline void forb::streams::socket::_move_attributes(forb::streams::socket &other) noexcept {
    this->_sock_fd        = other._sock_fd;
    this->_local_address  = other._local_address;
    this->_local_port     = other._local_port;
    this->_remote_address = other._remote_address;
    this->_remote_port    = other._remote_port;
    this->_is_server      = other._is_server;

    other._sock_fd        = 0;
    other._local_address  = "";
    other._local_port     = 0;
    other._remote_address = "";
    other._remote_port    = 0;
    other._is_server      = false;

    // Remember to move other members if necessary
}


/* ************************************************** CONSTRUCTORS ************************************************** */

forb::streams::socket::socket(forb::streams::socket::socket_create_t) {
    _sock_fd = ::socket(AF_TYPE, PROTOCOL, 0);

    assert(_sock_fd != 0);

    if (_sock_fd < 0) {
        throw forb::exception{"Error creating a new socket: " + std::string(std::strerror(errno)) + "."};
    }
}

forb::streams::socket::socket(int sock_fd,
                              const std::string &remote_address,
                              int remote_port)
        : _sock_fd(sock_fd),
          _remote_address(remote_address),
          _remote_port(remote_port) {

    // Checking whether the socket was created correctly
    assert(sock_fd > 0);
}

forb::streams::socket::socket(const std::string &remote_address, int remote_port)
        : socket(socket_create_t()) {
    this->_remote_address = remote_address;
    this->_remote_port    = remote_port;
}

/**********************************************************************************************************************/

forb::streams::socket::socket(forb::streams::socket &&other) noexcept {
    _move_attributes(other);
}

forb::streams::socket &forb::streams::socket::operator=(forb::streams::socket &&other) noexcept {
    if (this != &other) // Probably useless self-assignment check
    {
        // If we assign to an already opened socket,
        // the destination socket must be closed first.
        close();

        _move_attributes(other);
    }

    return *this;
}

/**********************************************************************************************************************/

void forb::streams::socket::bind(const std::string &address, int port) {
    assert(_sock_fd > 0);

    sockaddr_in_t local_address_structure;

    // Build struct sockaddr_in
    local_address_structure = build_sockaddr(address, port);

    // Force re-usage of already binded ports
    int option = 1;
    setsockopt(_sock_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    // Perform bind
    int res = ::bind(_sock_fd, reinterpret_cast<sockaddr_t *>(&local_address_structure), SOCKADDR_SIZE);

    if (res != 0) {
        throw forb::exception{
                "Error while binding address/port number to socket: "
                + std::string(std::strerror(errno))
                + "."
        };
    }

    // Store correctly performed action
    _local_address = address;
    _local_port    = port;
}

void forb::streams::socket::listen(int listen_queue_size) {
    assert(_sock_fd > 0);
    assert(_local_port > 0);

    int res = ::listen(_sock_fd, listen_queue_size);

    if (res != 0) {
        throw forb::exception{"Error while performing listen on socket: " + std::string(std::strerror(errno)) + "."};
    }

    _is_server = true;
}

void forb::streams::socket::connect(const std::string &address, int port) {
    assert(_sock_fd > 0);

    // Builds the structure
    sockaddr_in_t address_structure = build_sockaddr(address, port);

    // Performs connect
    int res = ::connect(_sock_fd,
                        reinterpret_cast<sockaddr_t *>(&address_structure), SOCKADDR_SIZE);

    if (res != 0) {
        throw forb::exception{"Error connecting to remote host: " + std::string(std::strerror(errno)) + "."};
    }

    // Store correctly performed action
    _remote_address = address;
    _remote_port    = port;
}

forb::streams::socket forb::streams::socket::accept() {
    assert(_sock_fd);

    if (!_is_server) {
        throw forb::exception{"Cannot accept on a non-server socket."};
    }

    sockaddr_in_t remote_address;
    socklen_t     sockaddr_size_var = SOCKADDR_SIZE;

    std::memset(&remote_address, 0, SOCKADDR_SIZE);

    // Perform accept operation
    int newsock_fd = ::accept(_sock_fd,
                              reinterpret_cast<struct sockaddr *>(&remote_address),
                              &sockaddr_size_var);

    assert(newsock_fd != 0);

    if (newsock_fd < 0) {
        throw forb::exception{
                "Error accepting a new connection on server socket: "
                + std::string(std::strerror(errno))
                + "."
        };
    }

    // Buffer used to store temporarily remote address
    char remote_addr_cstr[16];

    auto res = ::inet_ntop(AF_TYPE,
                           &remote_address,
                           remote_addr_cstr,
                           sockaddr_size_var);

    if (res == nullptr) {
        throw forb::exception{
                "Received a new connection request from invalid host: "
                + std::string(std::strerror(errno))
                + "."
        };
    }

    // Return new communication socket
    return socket(newsock_fd, std::string{remote_addr_cstr}, _remote_port);
}

void forb::streams::socket::send(const void *buffer, std::size_t size) {
    if (_sock_fd < 1) {
        throw forb::exception{"Tried to send data over invalid socket."};
    }

    if (_is_server) {
        throw forb::exception{"Tried to send data over server socket."};
    }

    // auto buffer_ptr    = reinterpret_cast<std::byte *>(buffer); // from C++17
    auto    buffer_ptr    = reinterpret_cast<const unsigned char *>(buffer); // from C++98
    size_t  remaining     = size;
    ssize_t how_many_sent = 0;

    while (remaining > 0) {
        how_many_sent = ::send(_sock_fd, buffer_ptr, remaining, 0);

        if (how_many_sent < 0) {
            throw forb::exception{
                    "Error while sending data: "
                    + std::string(std::strerror(errno))
                    + "."
            };
        }

        if (size_t(how_many_sent) > remaining) {
            throw forb::exception{"Sent more data than requested."};
        }

        remaining -= how_many_sent;
        buffer_ptr += how_many_sent;
    }

    assert(remaining == 0);
}

void forb::streams::socket::recv(void *buffer, std::size_t size) {
    if (_sock_fd < 1) {
        throw forb::exception{"Tried to receive data over invalid socket."};
    }

    if (_is_server) {
        throw forb::exception{"Tried to receive data over server socket."};
    }

    // auto buffer_ptr    = reinterpret_cast<std::byte *>(buffer); // from C++17
    auto    buffer_ptr    = reinterpret_cast<unsigned char *>(buffer); // from C++98
    size_t  remaining     = size;
    ssize_t how_many_recv = 0;

    while (remaining > 0) {
        how_many_recv = ::read(_sock_fd, buffer_ptr, remaining);

        if (how_many_recv < 0) {
            throw forb::exception{
                    "Error while receiving data: "
                    + std::string(std::strerror(errno))
                    + "."
            };
        }

        if (size_t(how_many_recv) > remaining) {
            throw forb::exception{"Received more data than requested."};
        }

        remaining -= how_many_recv;
        buffer_ptr += how_many_recv;
    }

    assert(remaining == 0);
}