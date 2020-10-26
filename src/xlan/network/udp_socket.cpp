// SPDX-License-Identifier: GPL-3.0-only

#ifdef __linux__
#include <sys/types.h>
#include <sys/socket.h>
#endif

#include "udp_socket.hpp"
#include "opaque_socket.hpp"

namespace XLAN::Network {
    std::optional<std::vector<std::vector<std::byte>>> UDPSocket::read_packets() {
        std::terminate(); // TODO
    }

    void UDPSocket::send_packet(const SocketAddress &to, const std::byte *data, std::size_t data_size) {
        std::terminate(); // TODO
    }

    const SocketAddress &UDPSocket::get_bound_address() const noexcept {
        return *this->address;
    }

    UDPSocket::UDPSocket(const SocketAddress &bind_to) :
        socket_ref(std::make_unique<OpaqueUDPSocket>(bind_to)),
        address(std::make_unique<SocketAddress>(bind_to)) {
    }

    UDPSocket::~UDPSocket() {

    }
}
