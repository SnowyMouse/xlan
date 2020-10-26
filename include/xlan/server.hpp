// SPDX-License-Identifier: GPL-3.0-only

#ifndef XLAN__SERVER_HPP
#define XLAN__SERVER_HPP

#include <list>
#include <optional>
#include <string>
#include <variant>
#include <vector>
#include <memory>

namespace XLAN {
    class Client;
    class SystemLinkPacket;
    class SocketAddress;

    namespace Network {
        class TCPStream;
        class TCPListener;
        class UDPSocket;
    }

    /**
     * A Server is used to facilitate communication between clients (peers). A Server instance can be either represent
     * a server hosted by the program or a remote server being connected to.
     *
     * If hosting a server, clients can be disconnected, given the operator status, renamed, etc. Server parameters
     * such as password or encryption can be modified.
     *
     * If connected to a server, the above actions can only be performed if the user is given operator status.
     */
    class Server {
    public:
        /** Client reference */
        using ClientReference = std::shared_ptr<Client>;

        /**
         * Peform a logic loop. This retrieves all queued packets from clients, accepts connections, delegate packets,
         * and so on. You need to run this in your program loop as often as possible.
         */
        void loop();

        /**
         * Get whether or not this instance is a host instance
         * @return true if a host instance, false if not
         */
        bool is_client() const noexcept { return this->client; };

        /**
         * Bind to a given address and start hosting.
         * @param tcp_bind TCP address to bind to
         * @param udp_bind UDP address to bind to
         */
        void host(const SocketAddress &tcp_bind, const SocketAddress &udp_bind);

        /**
         * Connect to the given
         * @param tcp_host TCP address to connect to
         * @param udp_host UDP address to connect to
         * @param tcp_bind TCP address to bind to
         * @param udp_bind UDP address to bind to
         * @param name     name to use; if null, a default name will be chosen by the server
         * @param password password to use; passing null is equivalent to passing an empty string in this case
         */
        void connect(
            const SocketAddress &tcp_host,
            const SocketAddress &udp_host,
            const std::optional<SocketAddress> &tcp_bind,
            const std::optional<SocketAddress> &udp_bind,
            const char *name = nullptr,
            const char *password = nullptr
        );

        /**
         * Get the name of the server. This pointer will be invalidated if set_name() or loop() is called or if the
         * Server instance is destroyed.
         *
         * @return name of the server
         */
        const char *get_name() const noexcept { return this->name.c_str(); };

        /**
         * Set the name of the server.
         *
         * If the user doesn't have permission, then Server::error_callback() will be called.
         *
         * @param new_name new server name
         */
        void set_name(const char *new_name);

        /**
         * Instantiate a server
         */
        Server() = default;

        /**
         * Destroy a server
         */
        virtual ~Server();

    protected:
        /**
         * This is called when a client has connected. If hosting, then this is called before clients are notified.
         * @param client connecting client
         */
        virtual void connection_callback(ClientReference client);

        /**
         * This is called when a client disconnects
         * @param client disconnecting client
         * @param reason reason for the client disconnecting
         */
        virtual void disconnection_callback(ClientReference client, const char *reason);

        /**
         * This is called when a client send a message
         * @param client  client sending the message, or nullptr if no client
         * @param message message being sent
         * @param allow   set to true to allow, false to not; ignored if not host
         */
        virtual void message_callback(std::optional<ClientReference> client, const char *message, bool &allow);

        /**
         * This is called when receiving a packet via system link
         * @param packet reference to packet data
         * @param allow  set to true to allow, false to not; ignored if not host
         */
        virtual void system_link_packet_callback(const SystemLinkPacket &packet, bool &allow);

    private:
        /** Clients in server */
        std::list<ClientReference> clients;

        /** Socket for transmitting TCP data if not host */
        std::unique_ptr<Network::TCPStream> tcp_stream;

        /** Socket for listening for TCP connections if host */
        std::unique_ptr<Network::TCPListener> tcp_listener;

        /** Socket for transmitting UDP packets */
        std::unique_ptr<Network::UDPSocket> udp;

        /** Are we a client instance? */
        bool client;

        /** Next client ID to assign */
        std::uint64_t next_client_id = 0;

        /** Name of the server */
        std::string name;

        /** Password of the server */
        std::string password;

        /** Buffer received from the client */
        std::vector<std::byte> recv_buffer;
    };
}

#endif
