// SPDX-License-Identifier: GPL-3.0-only

#include "tcp_stream.hpp"
#include "opaque_socket.hpp"

namespace XLAN::Network {
    std::optional<std::vector<std::byte>> TCPStream::read_bytes() {
        std::terminate(); // TODO
    }

    void TCPStream::send_bytes(const std::byte *data, std::size_t data_size) {
        std::terminate(); // TODO
    }

    const SocketAddress &TCPStream::get_bound_address() const noexcept {
        return *this->bound_address;
    }

    const SocketAddress &TCPStream::get_recipient_address() const noexcept {
        return *this->to_address;
    }

    TCPStream::TCPStream(const SocketAddress &to) :
        socket_ref(std::make_unique<OpaqueTCPStream>(to)),
        to_address(std::make_unique<SocketAddress>(to))
        {

        // If we're using BSD sockets, we need to retrieve the name information from the socket
        #ifdef USE_BSD_SOCKETS

        SocketAddress a;
        auto &ai = *a.address_data;
        socklen_t ai_l;
        getsockname(*this->socket_ref->s, reinterpret_cast<sockaddr *>(&ai.sockaddr), &ai_l);
        ai.address_length = ai_l;
        this->bound_address = std::make_unique<SocketAddress>(a);

        #else
        static_assert(false);
        #endif
    }

    TCPStream::TCPStream(const SocketAddress &to, const SocketAddress &bind_to) :
        socket_ref(std::make_unique<OpaqueTCPStream>(to, bind_to)),
        bound_address(std::make_unique<SocketAddress>(bind_to)),
        to_address(std::make_unique<SocketAddress>(to)) {}

    TCPStream::TCPStream() {}

    TCPStream::~TCPStream() {}
}
