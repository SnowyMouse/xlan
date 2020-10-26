// SPDX-License-Identifier: GPL-3.0-only

#ifndef XLAN__NETWORK__TCP_PACKET_HPP
#define XLAN__NETWORK__TCP_PACKET_HPP

#include <cstdint>

#include <xlan/client_id.hpp>
#include "endian.hpp"

namespace XLAN::Network {
    #define MAX_NAME_LENGTH 32
    #define REASON_LENGTH 64

    /**
     * Type of packet (put in header)
     */
    enum TCPType : std::uint16_t {
        TCPHandshake = 0xFEFF,
        TCPHandshakeResponse = 0xFF00,
        TCPConnectionInformation = 0xFF01,
        TCPConnectionInformationAcknowledged = 0xFF02,
        TCPConnectionRefused = 0xFFFF,

        TCPPing = 0,
        TCPPong = 1,
        TCPMessageSent = 2,
        TCPMessageReceived = 3,
        TCPUpdateUser = 4,
        TCPUserDisconnected = 5,
        TCPUDPPacket = 6,
        TCPUDPPacketReceived = 7
    };

    /**
     * This is just a header for the TCP packet. It's put before every TCP "packet"
     *
     * These packets aren't *actual* packets since TCP is a stream. So, these discrete structures can be sent in parts.
     */
    template <TCPType default_type> struct TCPPacket {
        /**
         * This is the packet type
         */
        NetworkEndian<std::uint16_t> type = default_type;

        static const TCPType DEFAULT_TYPE = default_type;
    };
    static_assert(sizeof(TCPPacket<TCPType::TCPHandshake>) == 2);

    /**
     * Handshake (sent from client to server)
     *
     * HandshakeResponse is expected after this.
     */
    struct Handshake : TCPPacket<TCPType::TCPHandshake> {
        /**
         * This is the expected version
         */
        static const std::uint32_t CURRENT_PROTOCOL_VERSION = 1;

        /**
         * Protocol version to use
         */
        NetworkEndian<std::uint32_t> protocol_version = CURRENT_PROTOCOL_VERSION;

        /**
         * Check to see if the protocol version is correct
         * @return true if protocol version is correct, false if not
         */
        bool verify() const noexcept { return this->protocol_version == CURRENT_PROTOCOL_VERSION; };
    };
    static_assert(sizeof(Handshake) == 6);

    /**
     * Handshake response (sent from server to client in response to a Handshake)
     *
     * ConnectionInformation is expected immediately after this
     */
    struct HandshakeResponse : TCPPacket<TCPType::TCPHandshakeResponse> {
        // TODO: Maybe use encryption?
    };
    static_assert(sizeof(HandshakeResponse) == 2);

    /**
     * Connection information (sent from client to server in response to a Handshake response)
     *
     * ClientInformationAcknowledged is expected after this
     */
    struct ConnectionInformation : TCPPacket<TCPType::TCPConnectionInformation> {
        /**
         * Name we want to use (UTF-8)
         *
         * The name we actually get is NOT guaranteed to be this, as the server may change it, especially if the name is already
         * in use.
         */
        std::int8_t requested_name[MAX_NAME_LENGTH] = {};

        /**
         * Password (bcrypt)
         */
        std::int8_t password[62] = {};

        /**
         * Check to see if the password is correct
         * @param password correct password
         * @return         true if password matches, false if not
         */
        bool verify(const char *password) const noexcept;

        /**
         * Put the password in the packet
         * @param password password to use
         */
        void set_password(const char *password) noexcept;
    };
    static_assert(sizeof(ConnectionInformation) == 96);

    /**
     * Client information acknowledged (sent from server to client)
     *
     * Ping is expected at least every 5 seconds (with a 15 second window)
     */
    struct ConnectionInformationAcknowledged : TCPPacket<TCPType::TCPConnectionInformationAcknowledged> {
        /**
         * This is the client ID of the client
         */
        NetworkEndian<ClientID> client_id;

        /**
         * UDP port of the server (65535 if UDP is disabled)
         */
        NetworkEndian<std::uint16_t> udp_port;
    };
    static_assert(sizeof(ConnectionInformationAcknowledged) == 12);

    /**
     * Connection refused (sent if handshake failed or if conneciton information is wrong)
     */
    struct ConnectionRefused : TCPPacket<TCPType::TCPConnectionRefused> {
        enum RefuseReason : std::uint32_t {
            ClientVersionTooOld = 0,
            ClientVersionTooNew = 1,
            ReceiveTimeout = 2
        };

        /**
         * Reason for the client to be refused
         */
        NetworkEndian<std::uint32_t> reason;
    };
    static_assert(sizeof(ConnectionRefused) == 6);

    struct Pong;
    /**
     * Ping (sent from server to client)
     *
     * A Pong is expected to be received immediately within 5 seconds containing the XOR of a and b.
     */
    struct Ping : TCPPacket<TCPType::TCPPing> {
        NetworkEndian<std::uint32_t> a;
        NetworkEndian<std::uint32_t> b;
    };
    static_assert(sizeof(Ping) == 10);

    /**
     * Pong (sent from client to server)
     *
     * xor_ab must be the xor of a and b. If it's wrong or the client takes longer than 5 seconds, the client is dropped.
     */
    struct Pong : TCPPacket<TCPType::TCPPong> {
        NetworkEndian<std::uint32_t> xor_ab;

        /**
         * Solve the ping
         */
        static Pong from_ping(const Ping &ping) noexcept {
            Pong r;
            r.xor_ab = ping.a ^ ping.b;
            return r;
        }
    };
    static_assert(sizeof(Pong) == 6);

    /**
     * Message (sent from client to server)
     *
     * The message text is sent immediately after this.
     */
    struct MessageSent : TCPPacket<TCPType::TCPMessageSent> {
        /**
         * Recipient client ID (INT64_MAX if it's to the main chat)
         */
        NetworkEndian<ClientID> recipient_id;

        /**
         * Length of message in bytes
         */
        NetworkEndian<std::uint16_t> message_length;
    };
    static_assert(sizeof(MessageSent) == 12);

    /**
     * Message (sent from client to server)
     *
     * The message text is sent (no null terminator) immediately after this.
     */
    struct MessageReceived : TCPPacket<TCPType::TCPMessageSent> {
        enum MessageReceivedFlags : std::uint8_t {
            /** Message is sent to the main chat */
            BROADCAST = 1 << 1
        };

        /**
         * Sender client ID (INT64_MAX if it's from the server)
         */
        NetworkEndian<ClientID> sender_id;

        /**
         * Message is public if 1
         */
        std::uint8_t flags;

        /**
         * Length of message in bytes
         */
        NetworkEndian<std::uint16_t> message_length;
    };
    static_assert(sizeof(MessageReceived) == 13);

    /**
     * Update user (sent from server to client)
     *
     * This is sent whenever a user joins, changes their name, or gets a different ping
     */
    struct UpdateUser : TCPPacket<TCPType::TCPUpdateUser> {
        /**
         * Client ID being updated
         */
        NetworkEndian<ClientID> client_id;

        /**
         * Name of client
         */
        std::int8_t name[MAX_NAME_LENGTH] = {};

        /**
         * Average ping of client
         */
        NetworkEndian<std::uint32_t> ping;
    };
    static_assert(sizeof(UpdateUser) == 46);

    /**
     * Disconnect user (sent from server to client)
     *
     * This is sent whenever a user disconnects
     */
    struct UserDisconnected : TCPPacket<TCPType::TCPUserDisconnected> {
        /**
         * Client ID being dropped
         */
        NetworkEndian<ClientID> client_id;

        /**
         * Disconnection reason
         */
        std::int8_t name[REASON_LENGTH] = {};
    };
    static_assert(sizeof(UserDisconnected) == 74);

    /**
     * UDP packet (sent from client to server)
     *
     * This is sent whenever a client sends a system link packet. The packet data is expected immediately afterwards.
     *
     * This won't be used if the server has UDP packets enabled.
     */
    struct UDPPacket : TCPPacket<TCPType::TCPUDPPacket> {
        /**
         * UDP packet length
         */
        NetworkEndian<std::uint16_t> packet_length;
    };
    static_assert(sizeof(UDPPacket) == 4);

    /**
     * UDP packet received (sent from server to client)
     *
     * This is sent whenever a client sends a system link packet. The packet data is expected immediately afterwards.
     */
    struct UDPPacketReceived : TCPPacket<TCPType::TCPUDPPacketReceived> {
        /**
         * Client ID sending the packet
         */
        NetworkEndian<ClientID> client_id;

        /**
         * UDP packet length
         */
        NetworkEndian<std::uint16_t> packet_length;
    };
    static_assert(sizeof(UDPPacketReceived) == 12);


}

#endif
