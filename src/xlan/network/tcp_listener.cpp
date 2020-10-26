// SPDX-License-Identifier: GPL-3.0-only

#include <unistd.h>

#include <xlan/network/socket_address.hpp>
#include "opaque_socket.hpp"
#include "tcp_listener.hpp"

namespace XLAN::Network {
    std::optional<std::unique_ptr<TCPStream>> TCPListener::accept_client() {
        #ifdef USE_BSD_SOCKETS

        // Check if we have bytes
        SocketAddress address;
        TCPStream::OpaqueTCPStream opaque_stream;

        address.address_data = std::make_unique<SocketAddress::OpaqueSocketAddress>();
        int sv = accept(*this->listener_ref->s, reinterpret_cast<sockaddr *>(&address.address_data->sockaddr), &address.address_data->address_length);
        if(sv == -1) {
            throw std::exception(); // TODO: put a meaningful error here
        }

        if(errno == EWOULDBLOCK) {
            return std::nullopt;
        }

        auto stream = std::unique_ptr<TCPStream>(new TCPStream);
        stream->bound_address = std::make_unique<SocketAddress>(address);
        stream->socket_ref = std::make_unique<TCPStream::OpaqueTCPStream>();
        stream->socket_ref->s = sv;
        return stream;

        #else

        static_assert(false);

        #endif
    }

    const SocketAddress &TCPListener::get_address() const noexcept {
        return *this->address;
    }

    TCPListener::TCPListener(const SocketAddress &bind_to) :
        listener_ref(std::make_unique<OpaqueTCPListenerSocket>(bind_to)),
        address(std::make_unique<SocketAddress>(bind_to))
        {}
}
