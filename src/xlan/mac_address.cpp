// SPDX-License-Identifier: GPL-3.0-only

#include <cstring>
#include <xlan/mac_address.hpp>

namespace XLAN {
    static constexpr const std::uint8_t BROADCAST_MAC_ADDRESS[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    MACAddress::MACAddress(const std::uint8_t address[6]) {
        std::memcpy(this->address, address, sizeof(this->address));
    }

    MACAddress MACAddress::broadcast() noexcept {
        return MACAddress(BROADCAST_MAC_ADDRESS);
    }

    bool MACAddress::is_broadcast() const noexcept {
        return std::memcmp(this->address, BROADCAST_MAC_ADDRESS, sizeof(this->address)) == 0;
    }

    bool MACAddress::can_send_to(const MACAddress &destination) const noexcept {
        return destination == *this || this->is_broadcast();
    }

    bool MACAddress::operator==(const MACAddress &other) const noexcept {
        return std::memcmp(this->address, other.address, sizeof(this->address)) == 0;
    }

    bool MACAddress::operator!=(const MACAddress &other) const noexcept {
        return !(*this == other);
    }
}
