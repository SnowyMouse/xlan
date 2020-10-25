// SPDX-License-Identifier: GPL-3.0-only

#ifndef XLAN__SYSTEM_LINK_PACKET_HPP
#define XLAN__SYSTEM_LINK_PACKET_HPP

#include <vector>

namespace XLAN {
    struct MACAddress;

    /**
     * This represents a system link packet
     */
    class SystemLinkPacket {
    public:
        /**
         * Convert to a raw packet. This can be encapsulated to be sent across the Internet or sent directly to a NIC.
         * @return raw bytes
         */
        std::vector<std::byte> to_raw() const;

        /**
         * Get the UDP packet payload.
         * @return payload bytes
         */
        std::vector<std::byte> get_udp_payload() const;

        /**
         * Get the physical address of the sender
         * @return sender
         */
        MACAddress get_source_mac_address() const noexcept;

        /**
         * Get the physical address of the intended recipient
         * @return recipient
         */
        MACAddress get_recipient_mac_address() const noexcept;
        
        /**
         * Check if the system link packet is valid
         * @param raw_data data to check
         * @param raw_size size to check
         * @param error    user-readable error code that describes what's wrong with the packet
         * @return         true if valid, false if not
         */
        static bool validate_raw_system_link_packet(const std::byte *raw_data, std::size_t raw_size, const char **error = nullptr);

        /**
         * Instantiate a system link packet with the given raw packet data
         * @param raw_data data length
         * @param raw_size data size
         */
        SystemLinkPacket(const std::byte *raw_data, std::size_t raw_size);

        SystemLinkPacket(const SystemLinkPacket &) = default;
    private:
        std::vector<std::byte> raw_data;
    };
}

#endif
