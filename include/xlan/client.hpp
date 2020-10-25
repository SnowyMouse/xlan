// SPDX-License-Identifier: GPL-3.0-only

#ifndef XLAN__CLIENT_HPP
#define XLAN__CLIENT_HPP

#include <optional>
#include <mutex>

#include "socket.hpp"
#include "clock.hpp"
#include "client_id.hpp"

namespace XLAN {
    class Server;

    /**
     * A Client is used to represent a peer.
     */
    class Client {
        friend class Server;

    public:
        /**
         * Attempt to drop the client from the server.
         *
         * If the user doesn't have permission, then Server::error_callback() will be called.
         *
         * If this is being called during Server::connection_callback() and the user is the host, then clients will not
         * be notified of the connection attempt or that the client was dropped.
         *
         * @param reason Reason to drop the client sent to all other clients
         */
        void drop(const char *reason = nullptr);

        /**
         * Attempt to op or de-op the client. This will allow the client to drop clients or op the client.
         *
         * If the user doesn't have permission, then Server::error_callback() will be called.
         *
         * @param reason Reason to op/deop the client sent to all other clients
         */
        void set_op(bool op, const char *reason = nullptr);

        /**
         * Get whether the client is an operator
         * @return true if the client is an operator
         */
        bool is_op() const noexcept { return this->opped; };

        /**
         * Privately send a message to the client
         * @param message message to send
         */
        void message(const char *message) const;

        /**
         * Calculate the ping of the player. If no ping is available, nullopt is returned.
         * @return ping of player
         */
        std::optional<std::uint32_t> get_ping() const noexcept;

        /**
         * Get the ID of this client
         * @return id of the client
         */
        ClientID get_client_id() const noexcept { return this->client_id; }

    protected:
        /**
         * Lock the mutex, waiting until it's unlocked if needed
         */
        void lock() const noexcept;

        /**
         * Unlock the mutex
         */
        void unlock() const noexcept;

        /**
         * Try to lock the mutex
         * @return true if locked, false if failed
         */
        bool try_lock() const noexcept;

    private:
        static const std::size_t MAX_PING = 5;

        /** Last moment the client was ping */
        Clock::time_point last_ping;

        /** Was the last ping successful? */
        bool last_ping_successful = true;

        /** Last five times the client was pinged */
        std::uint32_t pings[MAX_PING];

        /** Number of times the client was pinged, up to the maximum number of pings stored */
        std::size_t ping_count = 0;

        /** Socket for communicating with the client via TCP */
        std::optional<Socket> socket_tcp;

        /** Socket address (TCP) */
        std::optional<SocketAddress> socket_address_tcp;

        /** Socket address (UDP) */
        std::optional<SocketAddress> socket_address_udp;

        /** Server reference */
        Server &server;

        /** ID of the client */
        ClientID client_id;

        /** Is the client fully connected? */
        bool fully_connected = false;

        /** Is the client an operator? */
        bool opped = false;

        /** Mutex */
        mutable std::mutex mutex;

        /**
         * Instantiate the client
         * @param server server reference
         */
        Client(Server &server);
    };
}

#endif
