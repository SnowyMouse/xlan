// SPDX-License-Identifier: GPL-3.0-only

#ifdef __linux__
#include <sys/types.h>
#include <sys/socket.h>
#endif

#include "udp_socket.hpp"
#include "opaque_socket.hpp"

namespace XLAN::Network {
    std::vector<std::pair<std::vector<std::byte>, std::shared_ptr<SocketAddress>>> UDPSocket::read_packets() {
        std::vector<std::pair<std::vector<std::byte>, std::shared_ptr<SocketAddress>>> array;

        #ifdef USE_BSD_SOCKETS

        // Basically, loop until we stop receiving things
        while(true) {
            struct timeval tv = {};

            // Set our FD to our socket
            fd_set set;
            FD_ZERO(&set);
            FD_SET(*this->socket_ref->s, &set);

            // Check if we have bytes
            int sv = select(1, &set, nullptr, nullptr, &tv);
            if(sv == -1) {
                throw std::exception(); // TODO: put a meaningful error here
            }

            // We do
            else if(sv) {
                std::byte buffer[65536] = {};
                SocketAddress address;
                int received = recvfrom(*this->socket_ref->s, buffer, sizeof(buffer), 0, reinterpret_cast<sockaddr *>(&address.address_data->sockaddr), &address.address_data->address_length);
                if(received == -1) {
                    throw std::exception(); // TODO: put a meaningful error here
                }
                else {
                    array.emplace_back(std::vector<std::byte>(buffer, buffer + received), std::make_shared<SocketAddress>(address));
                }
            }

            // We don't
            else {
                return array;
            }
        }

        #else
        static_assert(false);
        #endif
    }

    void UDPSocket::send_packet(const SocketAddress &to, const std::byte *data, std::size_t data_size) {
        #ifdef USE_BSD_SOCKETS

        struct timeval tv = {};

        // Set our FD to our socket
        fd_set set;
        FD_ZERO(&set);
        FD_SET(*this->socket_ref->s, &set);

        // Check if we can send
        int sv = select(1, nullptr, &set, nullptr, &tv);
        if(sv == -1) {
            throw std::exception(); // TODO: put a meaningful error here
        }

        // We can
        else if(sv) {
            auto &send_to_addr = *to.address_data;
            int sent = sendto(*this->socket_ref->s, data, data_size, 0, reinterpret_cast<sockaddr *>(&send_to_addr.sockaddr), send_to_addr.address_length);
            if(sent == -1) {
                throw std::exception(); // TODO: put a meaningful error here
            }
            return;
        }

        // We don't
        else {
            throw std::exception(); // TODO: put a meaningful error here
        }

        #else
        static_assert(false);
        #endif
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
