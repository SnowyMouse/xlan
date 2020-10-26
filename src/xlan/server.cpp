// SPDX-License-Identifier: GPL-3.0-only

#include <xlan/server.hpp>
#include <xlan/client.hpp>

#include "network/tcp_listener.hpp"
#include "network/tcp_stream.hpp"
#include "network/udp_socket.hpp"

namespace XLAN {
    void Server::loop() {
        std::terminate(); // TODO
    }

    void Server::host(const SocketAddress &tcp_bind, const SocketAddress &udp_bind) {
        std::terminate(); // TODO
    }

    void Server::connect(
        const SocketAddress &tcp_host,
        const SocketAddress &udp_host,
        const std::optional<SocketAddress> &tcp_bind,
        const std::optional<SocketAddress> &udp_bind,
        const char *name,
        const char *password
    ) {
        std::terminate(); // TODO
    }

    void Server::set_name(const char *new_name) {
        std::terminate(); // TODO
    }

    Server::~Server() {

    }

    // Callbacks (by default they simply do nothing)
    void Server::connection_callback(ClientReference) {}
    void Server::disconnection_callback(ClientReference, const char *) {}
    void Server::message_callback(std::optional<ClientReference>, const char *, bool &) {}
    void Server::system_link_packet_callback(const SystemLinkPacket &, bool &) {}
}
