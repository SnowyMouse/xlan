// SPDX-License-Identifier: GPL-3.0-only

#ifndef XLAN__SOCKET_HPP
#define XLAN__SOCKET_HPP

#include <memory>

namespace XLAN {
    /**
     * This is a socket type which is used internally within XLAN. Since sockets aren't defined by C++ but are,
     * instead, implementation-defined (e.g. BSD sockets, winsock, etc.), an opaque pointer is used.
     */
    struct OpaqueSocket;

    /**
     * Socket represents a socket and is used for socket operations.
     */
    using Socket = std::shared_ptr<OpaqueSocket>;

    /**
     * This is a socket address type which is used internally within XLAN. Since socket addresses aren't defined by C++
     * but are, instead, implementation-defined (e.g. BSD sockets, winsock, etc.), an opaque pointer is used.
     */
    struct OpaqueSocketAddress;

    /**
     * SocketAddress represents a socket address and is used for socket operations.
     */
    using SocketAddress = std::shared_ptr<OpaqueSocketAddress>;
}

#endif
