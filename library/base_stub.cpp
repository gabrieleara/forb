//
// Created by gabriele on 04/11/18.
//

#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>

#include <forb/base_stub.hpp>

/* **************************************** UTILITY FUNCTIONS IMPLEMENTATION **************************************** */

/// The maximum length of an IP address.
static constexpr size_t MAX_IFADDRESS_LENGTH = (INET_ADDRSTRLEN > INET6_ADDRSTRLEN)
                                               ? INET_ADDRSTRLEN
                                               : INET6_ADDRSTRLEN;

/// Returns true if the given ip address is a valid IP address assigned to this machine.
static bool is_local_address(const std::string &ip) {
    struct ifaddrs *ifaddress_structure_list;
    struct ifaddrs *ifaddress_iterator;

    void *address_member_ptr;
    char address_buffer[MAX_IFADDRESS_LENGTH + 1]; // Just to be safe

    const char *request_ipaddress = ip.c_str();

    getifaddrs(&ifaddress_structure_list);

    // For each address in the linked list
    for (ifaddress_iterator = ifaddress_structure_list;
         ifaddress_iterator != nullptr;
         ifaddress_iterator = ifaddress_iterator->ifa_next) {

        // If it is not a valid address, skip.
        if (!ifaddress_iterator->ifa_addr) {
            continue;
        }

        if (ifaddress_iterator->ifa_addr->sa_family == AF_INET) {
            // We got a valid IPv4 address
            address_member_ptr = &((struct sockaddr_in *) ifaddress_iterator->ifa_addr)->sin_addr;

            // After this address_buffer will contain the IPv4 address
            inet_ntop(AF_INET, address_member_ptr, address_buffer, INET_ADDRSTRLEN);

        } else if (ifaddress_iterator->ifa_addr->sa_family == AF_INET6) {
            // We got a valid IPv6 address
            address_member_ptr = &((struct sockaddr_in6 *) ifaddress_iterator->ifa_addr)->sin6_addr;

            // After this address_buffer will contain the IPv6 address
            inet_ntop(AF_INET6, address_member_ptr, address_buffer, INET6_ADDRSTRLEN);

        } else {
            continue;
        }

        // Then we need to check whether it is the same as the given one
        if (strcmp(request_ipaddress, address_buffer) == 0) {
            freeifaddrs(ifaddress_structure_list);
            return true;
        }

        // Otherwise keep going on
    }

    if (ifaddress_structure_list != nullptr) {
        freeifaddrs(ifaddress_structure_list);
    }

    return false;
}

/* ********************************************** CLASS IMPLEMENTATION ********************************************** */

// For documentation see corresponding header file

void forb::base_stub::init_call(call_id_t code) {
    // First of all, initiate the remote call
    call_id_t stream_type = call_id_t_cast(datastream->get_type());

    // Set stream type information bit
    code |= stream_type;

    if (rpcstream->require_marshal()) {
        code = forb::streams::marshal(code);
    }

    // Serialize the code
    rpcstream->send(&code, sizeof(code));

    // In the case datastream is a shared memory, then its key shall be
    // serialized over the rpcstream.
    if (datastream->get_type() == stream::type::SHMEM) {
        shared_memory::key_t key = dynamic_cast<shared_memory *>(datastream)->get_key();

        if (rpcstream->require_marshal()) {
            key = forb::streams::marshal(key);
        }

        rpcstream->send(&key, sizeof(key));
    }
}

void forb::base_stub::wait_return() {
    forb::res_code_t res_code;
    rpcstream->recv(&res_code, sizeof(res_code));

    if (rpcstream->require_marshal()) {
        res_code = forb::streams::unmarshal(res_code);
    }
}


void forb::base_stub::set_skeleton(std::string address, int port) {
    _socket = ssocket::make_client(address, port);

    rpcstream = &_socket;

    // Any address will be considered remote if _force_socket is set
    bool is_local = !_force_socket && is_local_address(address);

    if (is_local) {
        _shmem     = shared_memory::create(_buffer_size);
        datastream = &_shmem;
    } else {
        datastream = rpcstream;
    }
}

std::vector<forb::base_stub *> &forb::base_stub::_protos() {
    static std::vector<forb::base_stub *> _prototypes;
    return _prototypes;
}

forb::remote_var forb::base_stub::_create(const std::string &type) {
    for (auto p : forb::base_stub::_protos()) {
        if (p->_match(type)) {
            return p->_create_empty();
        }
    }

    return nullptr;
}
