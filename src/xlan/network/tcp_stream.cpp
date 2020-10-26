// SPDX-License-Identifier: GPL-3.0-only

#include "tcp_stream.hpp"
#include "opaque_socket.hpp"

namespace XLAN::Network {
    std::vector<std::byte> TCPStream::read_bytes() {
        std::vector<std::byte> array;

        #ifdef USE_BSD_SOCKETS

        // Basically, loop until we stop receiving things
        while(true) {
            struct timeval tv = {};

            // Set our FD to our stream
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
                int received = recv(*this->socket_ref->s, buffer, sizeof(buffer), 0);
                if(received == -1) {
                    throw std::exception(); // TODO: put a meaningful error here
                }
                else {
                    array.insert(array.end(), buffer, buffer + received);
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

    void TCPStream::send_bytes(const std::byte *data, std::size_t data_size) {
        #ifdef USE_BSD_SOCKETS

        struct timeval tv = {};

        // Set our FD to our stream
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
            int sent = send(*this->socket_ref->s, data, data_size, 0);
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
        getsockname(*this->socket_ref->s, reinterpret_cast<sockaddr *>(&ai.sockaddr), &ai.address_length);
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
