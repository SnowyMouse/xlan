// SPDX-License-Identifier: GPL-3.0-only

#ifndef XLAN__NETWORK__OPAQUE_SOCKET_HPP
#define XLAN__NETWORK__OPAQUE_SOCKET_HPP

#include <xlan/network/socket_address.hpp>
#include <sys/socket.h>

#include "tcp_listener.hpp"
#include "tcp_stream.hpp"
#include "udp_socket.hpp"

#ifdef __linux__
#define USE_BSD_SOCKETS
#endif

#ifdef USE_BSD_SOCKETS
#include <unistd.h>

namespace XLAN {
    struct SocketAddress::OpaqueSocketAddress {
        sockaddr_storage sockaddr = {};
        std::size_t address_length = 0;
    };
}

namespace XLAN::Network {
    struct TCPListener::OpaqueTCPListenerSocket {
        std::optional<int> s;

        OpaqueTCPListenerSocket(const SocketAddress &address) {
            auto &addr_data = address.get_address_data();

            // Create socket
            int sv = socket(addr_data.sockaddr.ss_family, SOCK_STREAM, 0);
            if(sv == -1) {
                throw std::exception();
            }

            #ifdef __linux__

            // Reuse port (Linux thing)
            int opt = 1;
            setsockopt(sv, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

            #endif

            // Bind
            int bv = bind(sv, reinterpret_cast<const sockaddr *>(&addr_data.sockaddr), addr_data.address_length);
            if(bv == -1) {
                close(sv);
                throw std::exception();
            }

            // Listen
            listen(bv, 4);

            this->s = bv;
        }

        ~OpaqueTCPListenerSocket() {
            if(s.has_value()) {
                close(*s);
            }
        }
    };

    struct UDPSocket::OpaqueUDPSocket {
        std::optional<int> s;

        OpaqueUDPSocket(const SocketAddress &address) {
            auto &addr_data = address.get_address_data();

            // Create socket
            int sv = socket(addr_data.sockaddr.ss_family, SOCK_DGRAM, 0);
            if(sv == -1) {
                throw std::exception();
            }

            #ifdef __linux__

            // Reuse port (Linux thing)
            int opt = 1;
            setsockopt(sv, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

            #endif

            // Bind
            int bv = bind(sv, reinterpret_cast<const sockaddr *>(&addr_data.sockaddr), addr_data.address_length);
            if(bv == -1) {
                close(sv);
                throw std::exception();
            }

            this->s = sv;
        }

        ~OpaqueUDPSocket() {
            if(s.has_value()) {
                close(*s);
            }
        }
    };

    struct TCPStream::OpaqueTCPStream {
        std::optional<int> s;

        OpaqueTCPStream(const SocketAddress &address_to, const std::optional<SocketAddress> &bind_to = std::nullopt) {
            auto &to_addr_data = address_to.get_address_data();

            // Create socket
            int sv = socket(to_addr_data.sockaddr.ss_family, SOCK_STREAM, 0);
            if(sv == -1) {
                throw std::exception();
            }

            // Bind?
            if(bind_to.has_value()) {
                auto &from_addr_data = bind_to->get_address_data();
                int bv = bind(sv, reinterpret_cast<const sockaddr *>(&from_addr_data.sockaddr), from_addr_data.address_length);
                if(bv == -1) {
                    close(sv);
                    throw std::exception();
                }
            }

            // Connect
            int cv = connect(sv, reinterpret_cast<const sockaddr *>(&to_addr_data.sockaddr), to_addr_data.address_length);
            if(cv == -1) {
                close(sv);
                throw std::exception();
            }

            this->s = sv;
        }

        ~OpaqueTCPStream() {
            if(s.has_value()) {
                close(*s);
            }
        }
    };
}
#endif

#endif
