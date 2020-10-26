// SPDX-License-Identifier: GPL-3.0-only

#ifndef XLAN__NETWORK__UDP_SOCKET_HPP
#define XLAN__NETWORK__UDP_SOCKET_HPP

#include <vector>
#include <optional>
#include <memory>

namespace XLAN {
    class SocketAddress;
}

namespace XLAN::Network {
    /**
     * This is a non-blocking UDP socket
     */
    class UDPSocket {
    public:
        /**
         * Listen for packets and the corresponding addresses they originated from
         * @return packet(s) received
         */
        std::vector<std::pair<std::vector<std::byte>, std::shared_ptr<SocketAddress>>> read_packets();

        /**
         * Send a packet to the specified address
         * @param to        address to send to
         * @param data      data to send
         * @param data_size length of data to send
         */
        void send_packet(const SocketAddress &to, const std::byte *data, std::size_t data_size);

        /**
         * Get the address we are bound to
         */
        const SocketAddress &get_bound_address() const noexcept;

        /**
         * Create a UDP socket
         * @param bind_to socket to bind to
         */
        UDPSocket(const SocketAddress &bind_to);

        ~UDPSocket();

    private:
        /**
         * This is a UDP socket type which is used internally within XLAN. Since sockets aren't defined by C++
         * but are, instead, implementation-defined (e.g. BSD sockets, winsock, etc.), an opaque pointer is used.
         */
        struct OpaqueUDPSocket;

        /**
         * Socket
         */
        std::unique_ptr<OpaqueUDPSocket> socket_ref;

        /**
         * Address
         */
        std::unique_ptr<SocketAddress> address;
    };
}

#endif
