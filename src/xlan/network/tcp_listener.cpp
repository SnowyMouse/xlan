// SPDX-License-Identifier: GPL-3.0-only

#include <unistd.h>

#include <xlan/network/socket_address.hpp>
#include "opaque_socket.hpp"
#include "tcp_listener.hpp"

namespace XLAN::Network {
    std::optional<std::unique_ptr<TCPStream>> TCPListener::accept_client() {
        std::terminate(); // TODO
    }

    const SocketAddress &TCPListener::get_address() const noexcept {
        return *this->address;
    }

    TCPListener::TCPListener(const SocketAddress &bind_to) :
        listener_ref(std::make_unique<OpaqueTCPListenerSocket>(bind_to)),
        address(std::make_unique<SocketAddress>(bind_to))
        {}
}
