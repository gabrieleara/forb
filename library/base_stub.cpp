//
// Created by gabriele on 04/11/18.
//

#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>

#include "forb/base_stub.hpp"

constexpr size_t MAX_IFADDRESS_LENGTH = (INET_ADDRSTRLEN > INET6_ADDRSTRLEN)
                                        ? INET_ADDRSTRLEN
                                        : INET6_ADDRSTRLEN;

using base_stub = forb::base_stub;

bool base_stub::is_local_address(const std::string &ip) {
    struct ifaddrs *ifaddress_structure_list;
    struct ifaddrs *ifaddress_iterator;
    void           *address_member_ptr;
    char           address_buffer[MAX_IFADDRESS_LENGTH];
    const char     *request_ipaddress = ip.c_str();

    getifaddrs(&ifaddress_structure_list);

    for (ifaddress_iterator = ifaddress_structure_list;
         ifaddress_iterator != nullptr;
         ifaddress_iterator = ifaddress_iterator->ifa_next) {

        if (!ifaddress_iterator->ifa_addr) {
            continue;
        }

        if (ifaddress_iterator->ifa_addr->sa_family == AF_INET) {
            // We got a valid IPv4 address
            address_member_ptr = &((struct sockaddr_in *)
                    ifaddress_iterator->ifa_addr)->sin_addr;
            // After this address_buffer will contain the IPv4 address
            inet_ntop(AF_INET, address_member_ptr, address_buffer, INET_ADDRSTRLEN);
        } else if (ifaddress_iterator->ifa_addr->sa_family == AF_INET6) {
            // We got a valid IPv4 address
            address_member_ptr = &((struct sockaddr_in6 *)
                    ifaddress_iterator->ifa_addr)->sin6_addr;
            // After this address_buffer will contain the IPv6 address
            inet_ntop(AF_INET6, address_member_ptr, address_buffer, INET6_ADDRSTRLEN);
        } else {
            continue;
        }

        // Then we need to check whether it is the same as the given one
        if (strcmp(request_ipaddress, address_buffer) == 0) {
            if (ifaddress_structure_list != NULL)
                freeifaddrs(ifaddress_structure_list);
            return true;
        }

        // Otherwise keep going on
    }

    if (ifaddress_structure_list != NULL)
        freeifaddrs(ifaddress_structure_list);

    return false;
}


void base_stub::init_call(call_id_t code) {
    // First of all, initiate the remote call
    call_id_t stream_type = call_id_t_cast(datastream->get_type());

    code |= stream_type;

    if (rpcstream->require_marshal()) {
        code = forb::streams::marshal(code);
    }

    rpcstream->send(&code, sizeof(code));

    // Since now I use socket, I don't need additional code
    // If I used shmem I should send simply the shmem key
    // over the rpcstream communication
    if (datastream->get_type() == stream::type::SHMEM) {
        shared_memory::key_t key = dynamic_cast<shared_memory *>(datastream)->get_key();
        key = rpcstream->require_marshal() ? forb::streams::marshal(key) : key;
        rpcstream->send(&key, sizeof(key));
    }
}

void base_stub::wait_return() {
    // TODO: change type of rescode
    uint16_t rescode;
    rpcstream->recv(&rescode, sizeof(rescode));

    if (rpcstream->require_marshal())
        rescode = forb::streams::unmarshal(rescode);

    // TODO: do something with this code
}

std::vector<base_stub *> &base_stub::_protos() {
    static std::vector<base_stub *> _prototypes;
    return _prototypes;
}

base_stub *base_stub::_create(const std::string &type) {
    for (auto p : base_stub::_protos()) {
        if (p->_match(type)) {
            return p->_create_empty();
        }
    }

    throw 42;
}

