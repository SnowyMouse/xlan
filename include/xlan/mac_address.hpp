// SPDX-License-Identifier: GPL-3.0-only

#ifndef XLAN__MAC_ADDRESS_HPP
#define XLAN__MAC_ADDRESS_HPP

#include <cstdint>

namespace XLAN {
    /**
     * This contains a MAC address
     */
    struct MACAddress {
        /**
         * Physical address value
         */
        std::uint8_t address[6] = {};

        /**
         * Instantiate with a pointer to a physical address
         * @param address address to use
         */
        MACAddress(const std::uint8_t address[6]);

        /**
         * Instantiate a broadcast MAC address
         * @return broadcast MAC address
         */
        static MACAddress broadcast() noexcept;

        /**
         * Check if this is a broadcast MAC address
         * @return true if broadcast MAC address
         */
        bool is_broadcast() const noexcept;

        /**
         * Check if this MAC address can send packets to the destination MAC address
         * @param  destination destination physical address
         * @return             true if it can send to this address
         */
        bool can_send_to(const MACAddress &destination) const noexcept;

        MACAddress() = default;
        MACAddress(const MACAddress &) = default;
        MACAddress(MACAddress &&) = default;

        bool operator==(const MACAddress &other) const noexcept;
        bool operator!=(const MACAddress &other) const noexcept;
    };
}

#endif
