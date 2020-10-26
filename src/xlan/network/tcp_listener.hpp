// SPDX-License-Identifier: GPL-3.0-only

#ifndef XLAN__NETWORK__TCP_LISTENER_HPP
#define XLAN__NETWORK__TCP_LISTENER_HPP

#include <memory>

namespace XLAN {
    class SocketAddress;
}

namespace XLAN::Network {
    class TCPStream;

    /**
     * This is a non-blocking TCP listener which is used for listening for clients
     */
    class TCPListener {
    public:
        /**
         * Listen for a client
         * @return client if found
         */
        std::optional<std::unique_ptr<TCPStream>> accept_client();

        /**
         * Get the address
         */
        const SocketAddress &get_address() const noexcept;

        /**
         * Bind a TCP listener
         * @param bind_to address to bind to
         */
        TCPListener(const SocketAddress &bind_to);

        ~TCPListener();

    private:
        /**
         * This is a listener socket type which is used internally within XLAN. Since sockets aren't defined by C++
         * but are, instead, implementation-defined (e.g. BSD sockets, winsock, etc.), an opaque pointer is used.
         */
        struct OpaqueTCPListenerSocket;

        /**
         * Listener
         */
        std::unique_ptr<OpaqueTCPListenerSocket> listener_ref;

        /**
         * Address
         */
        std::unique_ptr<SocketAddress> address;
    };
}

#endif
