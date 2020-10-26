// SPDX-License-Identifier: GPL-3.0-only

#ifndef XLAN__NETWORK__SOCKET_ADDRESS_HPP
#define XLAN__NETWORK__SOCKET_ADDRESS_HPP

#include <memory>

namespace XLAN::Network {
    class TCPStream;
    class TCPListener;
    class UDPSocket;
}

namespace XLAN {
    class SocketAddress {
        friend class Network::TCPStream;
        friend class Network::TCPListener;
        friend class Network::UDPSocket;

    public:
        /**
         * This is a socket address type which is used internally within XLAN. Since socket addresses aren't defined by C++
         * but are, instead, implementation-defined (e.g. BSD sockets, winsock, etc.), an opaque pointer is used.
         */
        struct OpaqueSocketAddress;

        /**
         * IP version
         */
        enum IPVersion {
            IPv4,
            IPv6,
            AnyIPVersion
        };

        /**
         * Error that happens if a hostname failed to resolve
         */
        class ResolveFailure : public std::exception {
            friend class SocketAddress;
        public:
            const char *what() const noexcept override {
                return error.c_str();
            }
        private:
            std::string error;
        };

        /**
         * Get whether or not this is IPv4 or IPv6
         * @return IP version
         */
        IPVersion get_ip_version() const noexcept;

        /**
         * Get a reference to the internal address data
         * @return address data
         */
        const OpaqueSocketAddress &get_address_data() const noexcept;

        /**
         * Resolve an address and port into a SocketAddress
         * @param address    address to resolve
         * @param port       port to use
         * @param ip_version IP version(s) to use
         * @throws           ResolveFailure if failed
         */
        SocketAddress(const char *address, std::uint16_t port, IPVersion ip_version = AnyIPVersion);

        SocketAddress(const SocketAddress &other);
        SocketAddress(SocketAddress &&other);
        ~SocketAddress();

    private:
        /**
         * Address data
         */
        std::unique_ptr<OpaqueSocketAddress> address_data;
        SocketAddress();
    };
}

#endif
