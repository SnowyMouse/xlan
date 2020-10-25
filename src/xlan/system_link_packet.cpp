// SPDX-License-Identifier: GPL-3.0-only

#include <xlan/mac_address.hpp>
#include <xlan/system_link_packet.hpp>
#include <stdexcept>
#include <bit>
#include <type_traits>

#include "network/endian.hpp"

namespace XLAN {
    static constexpr const std::size_t ETHERNET_DESTINATION_OFFSET          = 0;
    static constexpr const std::size_t ETHERNET_SOURCE_OFFSET               = 6;
    static constexpr const std::size_t ETHERNET_TYPE_OFFSET                 = 12;
    static constexpr const std::size_t ETHERNET_ETHERNET_SIZE               = 14;
    static constexpr const std::size_t ETHERNET_IPV4_OFFSET                 = ETHERNET_ETHERNET_SIZE;
    
    static constexpr const std::size_t ETHERNET_IPV4_HV_OFFSET              = ETHERNET_IPV4_OFFSET + 0;
    static constexpr const std::size_t ETHERNET_IPV4_LENGTH_OFFSET          = ETHERNET_IPV4_OFFSET + 2;
    static constexpr const std::size_t ETHERNET_IPV4_PROTOCOL_OFFSET        = ETHERNET_IPV4_OFFSET + 9;
    static constexpr const std::size_t ETHERNET_IPV4_SOURCE_IP_OFFSET       = ETHERNET_IPV4_OFFSET + 12;
    static constexpr const std::size_t ETHERNET_IPV4_DESTINATION_IP_OFFSET  = ETHERNET_IPV4_OFFSET + 16;
    static constexpr const std::size_t IPV4_MINIMUM_SIZE                    = 20;
    static constexpr const std::size_t ETHERNET_IPV4_MINIMUM_SIZE           = ETHERNET_IPV4_OFFSET + IPV4_MINIMUM_SIZE;

    static constexpr const std::size_t UDP_SOURCE_PORT_OFFSET               = 0;
    static constexpr const std::size_t UDP_DESTINATION_PORT_OFFSET          = 2;
    static constexpr const std::size_t UDP_LENGTH_OFFSET                    = 4;
    static constexpr const std::size_t UDP_PAYLOAD_OFFSET                   = 8;
        
    #define READ_RAW_DATA_INT(offset, T) Network::network_to_host(*reinterpret_cast<const T *>(raw_data + offset))
    
    static std::size_t sl_udp_offset(const std::byte *raw_data, std::size_t raw_data_size, const char **error = nullptr) {
        // If error is nullptr, set it to something that is valid. This is just to save us from having to check if it's null so many times.
        const char *t = nullptr;
        if(error == nullptr) {
            error = &t;
        }
        
        // Can it even contain a full IPv4 packet?
        if(raw_data_size < ETHERNET_IPV4_MINIMUM_SIZE) {
            *error = "XLAN::sl_udp_offset(): SL packet too small to be an IPv4 packet";
            return 0;
        };
        
        // Is it IPv4?
        auto hv = READ_RAW_DATA_INT(ETHERNET_IPV4_HV_OFFSET, std::uint8_t);
        if(((hv & 0b1111) != 4) || READ_RAW_DATA_INT(ETHERNET_TYPE_OFFSET, std::uint16_t) != 8) {
            *error = "XLAN::sl_udp_offset(): SL packet is not IPv4";
            return 0;
        }
        
        // Is it UDP?
        if(READ_RAW_DATA_INT(ETHERNET_IPV4_PROTOCOL_OFFSET, std::uint8_t) != 0x11) {
            *error = "XLAN::sl_udp_offset(): SL packet is not UDP";
            return 0;
        }
        
        // Is the ipv4 length correct
        auto ipv4_packet_size = static_cast<std::size_t>(READ_RAW_DATA_INT(ETHERNET_IPV4_LENGTH_OFFSET, std::uint16_t));
        if(ipv4_packet_size + ETHERNET_IPV4_OFFSET != raw_data_size) {
            *error = "XLAN::sl_udp_offset(): SL packet ipv4 size is wrong";
            return 0;
        }
        
        // Is the UDP stuff out of bounds?
        auto udp_offset = static_cast<std::size_t>(hv >> 4) + ETHERNET_IPV4_OFFSET;
        if(udp_offset > raw_data_size) {
            *error = "XLAN::sl_udp_offset(): SL packet is too small to be a UDP packet";
            return 0;
        }
        
        return udp_offset;
    }
    

    std::vector<std::byte> SystemLinkPacket::to_raw() const {
        return this->raw_data;
    }

    MACAddress SystemLinkPacket::get_source_mac_address() const noexcept {
        return MACAddress(reinterpret_cast<const std::uint8_t *>(this->raw_data.data() + ETHERNET_SOURCE_OFFSET));
    }

    MACAddress SystemLinkPacket::get_recipient_mac_address() const noexcept {
        return MACAddress(reinterpret_cast<const std::uint8_t *>(this->raw_data.data() + ETHERNET_DESTINATION_OFFSET));
    }

    std::vector<std::byte> SystemLinkPacket::get_udp_payload() const {
        return std::vector<std::byte>(this->raw_data.begin() + sl_udp_offset(this->raw_data.data(), this->raw_data.size()) + UDP_PAYLOAD_OFFSET, this->raw_data.end());
    }
    
    bool SystemLinkPacket::validate_raw_system_link_packet(const std::byte *raw_data, std::size_t raw_size, const char **error) {
        // If error is nullptr, set it to something that is valid. This is just to save us from having to check if it's null so many times.
        const char *t = nullptr;
        if(error == nullptr) {
            error = &t;
        }
        
        // Get the UDP offset (includes most other checks)
        auto udp_offset = sl_udp_offset(raw_data, raw_size, error);
        if(*error != nullptr) {
            return false;
        }
        
        // Are the ports correct?
        auto source_port = READ_RAW_DATA_INT(udp_offset + UDP_SOURCE_PORT_OFFSET, std::uint16_t);
        auto destination_port = READ_RAW_DATA_INT(udp_offset + UDP_DESTINATION_PORT_OFFSET, std::uint16_t);
        if(source_port != 3074) {
            *error = "XLAN::SystemLinkPacket(): SL source port is not 3074";
            return false;
        }
        if(destination_port != 3074) {
            *error = "XLAN::SystemLinkPacket(): SL destination port is not 3074";
            return false;
        }
        
        // Are the IPs correct?
        auto source_ip = READ_RAW_DATA_INT(ETHERNET_IPV4_SOURCE_IP_OFFSET, std::uint32_t);
        auto destination_ip = READ_RAW_DATA_INT(ETHERNET_IPV4_DESTINATION_IP_OFFSET, std::uint32_t);
        
        // Source IP must ALWAYS be 0.0.0.1
        if(source_ip != 0x00000001) {
            *error = "XLAN::SystemLinkPacket(): SL source IP is not 0.0.0.1";
            return false;
        }
        
        // Source address must not be broadcast
        if(MACAddress(reinterpret_cast<const std::uint8_t *>(raw_data + ETHERNET_SOURCE_OFFSET)).is_broadcast()) {
            *error = "XLAN::SystemLinkPacket(): SL source MAC address is broadcast";
            return false;
        }
        
        // Destination IP must be 0.0.0.1 if non-broadcast. Otherwise it must be 255.255.255.255.
        if(MACAddress(reinterpret_cast<const std::uint8_t *>(raw_data + ETHERNET_DESTINATION_OFFSET)).is_broadcast()) {
            if(destination_ip != 0xFFFFFFFF) {
                *error = "XLAN::SystemLinkPacket(): SL destination IP is not 255.255.255.255 but is broadcast";
                return false;
            }
        }
        else {
            if(destination_ip != 0x00000001) {
                *error = "XLAN::SystemLinkPacket(): SL destination IP is not 0.0.0.1";
                return false;
            }
        }
        
        // Is the size right?
        if(READ_RAW_DATA_INT(udp_offset + UDP_LENGTH_OFFSET, std::uint16_t) + udp_offset != raw_size) {
            *error = "XLAN::SystemLinkPacket(): SL UDP payload size is wrong";
            return false;
        }
        
        return true;
    }

    SystemLinkPacket::SystemLinkPacket(const std::byte *raw_data, std::size_t raw_size) {
        // Validate it
        const char *error;
        if(!validate_raw_system_link_packet(raw_data, raw_size, &error)) {
            throw std::invalid_argument(error);
        }
        
        // Works for me
        this->raw_data = std::vector<std::byte>(raw_data, raw_data + raw_size);
    }
            
    #undef READ_RAW_DATA_INT
}
