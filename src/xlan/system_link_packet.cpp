// SPDX-License-Identifier: GPL-3.0-only

#include <xlan/mac_address.hpp>
#include <xlan/system_link_packet.hpp>
#include <stdexcept>
#include <bit>
#include <type_traits>

#include "network/endian.hpp"

namespace XLAN {
    using namespace Network;
    
    struct EthernetHeader {
        MACAddress destination_mac;
        MACAddress source_mac;
        NetworkEndian<std::uint16_t> type;
    };
    static_assert(sizeof(EthernetHeader) == 14);
    
    struct IPv4Header : EthernetHeader {
        NetworkEndian<std::uint8_t> version_ihl;
        NetworkEndian<std::uint8_t> dscp_ecn;
        NetworkEndian<std::uint16_t> ipv4_length;
        NetworkEndian<std::uint16_t> identification;
        NetworkEndian<std::uint16_t> fragment;
        NetworkEndian<std::uint8_t> ttl;
        NetworkEndian<std::uint8_t> protocol;
        NetworkEndian<std::uint16_t> checksum;
        NetworkEndian<std::uint32_t> source_ip;
        NetworkEndian<std::uint32_t> destination_ip;
    };
    static_assert(sizeof(IPv4Header) == sizeof(EthernetHeader) + 20);
    
    struct UDPHeader {
        NetworkEndian<std::uint16_t> source_port;
        NetworkEndian<std::uint16_t> destination_port;
        NetworkEndian<std::uint16_t> length;
        NetworkEndian<std::uint16_t> checksum;
    };
    static_assert(sizeof(UDPHeader) == 8);
    
    
    static std::size_t sl_udp_offset(const std::byte *raw_data, std::size_t raw_data_size, const char **error = nullptr) {
        // If error is nullptr, set it to something that is valid. This is just to save us from having to check if it's null so many times.
        const char *t = nullptr;
        if(error == nullptr) {
            error = &t;
        }
        
        // Can it even contain a full IPv4 packet?
        if(raw_data_size < sizeof(IPv4Header)) {
            *error = "XLAN::sl_udp_offset(): SL packet too small to be an IPv4 packet";
            return 0;
        };
        
        const auto &ipv4_header = *reinterpret_cast<const IPv4Header *>(raw_data);
        
        // Is it IPv4?
        auto hv = static_cast<std::uint8_t>(ipv4_header.version_ihl);
        if(((hv & 0b1111) != 4) || ipv4_header.type != 8) {
            *error = "XLAN::sl_udp_offset(): SL packet is not IPv4";
            return 0;
        }
        
        // Is it UDP?
        if(ipv4_header.protocol != 0x11) {
            *error = "XLAN::sl_udp_offset(): SL packet is not UDP";
            return 0;
        }
        
        // Is the ipv4 length correct
        auto ipv4_packet_size = static_cast<std::size_t>(ipv4_header.ipv4_length);
        if(ipv4_packet_size + sizeof(EthernetHeader) != raw_data_size) {
            *error = "XLAN::sl_udp_offset(): SL packet ipv4 size is wrong";
            return 0;
        }
        
        // Is the UDP stuff out of bounds?
        auto udp_offset = static_cast<std::size_t>(hv >> 4) + sizeof(EthernetHeader);
        if(udp_offset > raw_data_size) {
            *error = "XLAN::sl_udp_offset(): SL packet is too small to be a UDP packet";
            return 0;
        }
        
        // Source IP must ALWAYS be 0.0.0.1
        if(ipv4_header.source_ip != 0x00000001) {
            *error = "XLAN::sl_udp_offset(): SL source IP is not 0.0.0.1";
            return 0;
        }
        
        // Source address must not be broadcast
        if(ipv4_header.source_mac.is_broadcast()) {
            *error = "XLAN::sl_udp_offset(): SL source MAC address is broadcast";
            return 0;
        }
        
        // Destination IP must be 0.0.0.1 if non-broadcast. Otherwise it must be 255.255.255.255.
        if(ipv4_header.destination_mac.is_broadcast()) {
            if(ipv4_header.destination_ip != 0xFFFFFFFF) {
                *error = "XLAN::sl_udp_offset(): SL destination IP is not 255.255.255.255 but is broadcast";
                return 0;
            }
        }
        else {
            if(ipv4_header.destination_ip != 0x00000001) {
                *error = "XLAN::sl_udp_offset(): SL destination IP is not 0.0.0.1";
                return 0;
            }
        }
        
        return udp_offset;
    }
    

    std::vector<std::byte> SystemLinkPacket::to_raw() const {
        return this->raw_data;
    }

    MACAddress SystemLinkPacket::get_source_mac_address() const noexcept {
        return MACAddress(reinterpret_cast<const EthernetHeader *>(this->raw_data.data())->source_mac);
    }

    MACAddress SystemLinkPacket::get_recipient_mac_address() const noexcept {
        return MACAddress(reinterpret_cast<const EthernetHeader *>(this->raw_data.data())->destination_mac);
    }

    std::vector<std::byte> SystemLinkPacket::get_udp_payload() const {
        return std::vector<std::byte>(this->raw_data.begin() + sl_udp_offset(this->raw_data.data(), this->raw_data.size()) + sizeof(UDPHeader), this->raw_data.end());
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
        
        const auto &udp_header = *reinterpret_cast<const UDPHeader *>(raw_data + udp_offset);
        
        // Are the ports correct?
        if(udp_header.source_port != 3074) {
            *error = "XLAN::SystemLinkPacket(): SL source port is not 3074";
            return false;
        }
        if(udp_header.destination_port != 3074) {
            *error = "XLAN::SystemLinkPacket(): SL destination port is not 3074";
            return false;
        }
        
        // Is the size right?
        if(udp_header.length + udp_offset != raw_size) {
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
