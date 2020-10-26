// SPDX-License-Identifier: GPL-3.0-only

#include <xlan/network/socket_address.hpp>

#include <cstring>

#include "opaque_socket.hpp"

namespace XLAN {
    const SocketAddress::OpaqueSocketAddress &SocketAddress::get_address_data() const noexcept {
        return *this->address_data;
    }

    SocketAddress::SocketAddress(const SocketAddress &other) :
        address_data(std::make_unique<OpaqueSocketAddress>(*other.address_data)) {}

    SocketAddress::SocketAddress(SocketAddress &&other) :
        address_data(std::move(other.address_data)) {}

    SocketAddress::SocketAddress() {}
    SocketAddress::~SocketAddress() {}
}

// BSD sockets
#ifdef USE_BSD_SOCKETS
#include <sys/socket.h>
#include <netdb.h>

namespace XLAN {
    SocketAddress::SocketAddress(const char *address, std::uint16_t port, IPVersion ip_version) {
        addrinfo *result;
        addrinfo hints = {};

        // Set the IP version
        switch(ip_version) {
            case IPVersion::AnyIPVersion:
                hints.ai_family = AF_UNSPEC;
                break;
            case IPVersion::IPv6:
                hints.ai_family = AF_INET6;
                break;
            case IPVersion::IPv4:
                hints.ai_family = AF_INET;
                break;
        }

        hints.ai_flags = AI_NUMERICSERV;
        char service[64];
        std::snprintf(service, sizeof(service), "%u", port);

        // Get the address
        int addrinfo_result = getaddrinfo(address, service, &hints, &result);
        if(addrinfo_result != 0) {
            ResolveFailure r;
            r.error = gai_strerror(addrinfo_result);
            throw r;
        }

        // Okay let's do this
        this->address_data = std::make_unique<OpaqueSocketAddress>();
        this->address_data->address_length = result->ai_addrlen;
        std::memcpy(&this->address_data->sockaddr, &result->ai_addr, this->address_data->address_length);

        freeaddrinfo(result);
    }
}

#else
static_assert(false);
#endif
