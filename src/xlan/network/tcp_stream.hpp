// SPDX-License-Identifier: GPL-3.0-only

#ifndef XLAN__NETWORK__TCP_STREAM_HPP
#define XLAN__NETWORK__TCP_STREAM_HPP

#include <vector>
#include <cstddef>
#include <optional>
#include <memory>

namespace XLAN {
    class SocketAddress;
}

namespace XLAN::Network {
    class TCPListener;

    /**
     * This is a non-blocking TCP stream which is used for sending/receiving a stream of bytes
     */
    class TCPStream {
        friend class TCPListener;
    public:
        /**
         * Listen for bytes
         * @return bytes if received
         */
        std::optional<std::vector<std::byte>> read_bytes();

        /**
         * Send bytes
         * @param data      data to send
         * @param data_size length of data in bytes
         */
        void send_bytes(const std::byte *data, std::size_t data_size);

        /**
         * Get the address we are bound to
         */
        const SocketAddress &get_bound_address() const noexcept;

        /**
         * Get the address we are sending data to
         */
        const SocketAddress &get_recipient_address() const noexcept;

        /**
         * Create a TCP stream
         * @param to socket to transmit to
         */
        TCPStream(const SocketAddress &to);

        /**
         * Create a TCP stream
         * @param to      socket to transmit to
         * @param bind_to address to bind to
         */
        TCPStream(const SocketAddress &to, const SocketAddress &bind_to);

        ~TCPStream();

    protected:
        /**
         * Instantiate a TCP stream that isn't used by anything
         */
        TCPStream();

    private:
        /**
         * This is a TCP socket type which is used internally within XLAN. Since sockets aren't defined by C++
         * but are, instead, implementation-defined (e.g. BSD sockets, winsock, etc.), an opaque pointer is used.
         */
        struct OpaqueTCPStream;

        /**
         * Socket
         */
        std::unique_ptr<OpaqueTCPStream> socket_ref;

        /**
         * Address
         */
        std::unique_ptr<SocketAddress> bound_address;

        /**
         * Address
         */
        std::unique_ptr<SocketAddress> to_address;
    };
}

#endif
