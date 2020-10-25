// SPDX-License-Identifier: GPL-3.0-only

#ifndef XLAN__NETWORK__ENDIAN_HPP
#define XLAN__NETWORK__ENDIAN_HPP

namespace XLAN::Network {
    template <typename T> constexpr T swap_endianness(T value) {
        static_assert(std::is_integral<T>::value);
        
        // If the size of the value is one byte, pass as-is
        if(sizeof(value) == 1) {
            return value;
        }
        
        else {
            T flipped = 0;
            for(std::size_t i = 0; i < sizeof(value); i++) {
                flipped = (flipped << 8) | (value & 0xFF);
                value >>= 8;
            }
            return flipped;
        }
    }
    
    static_assert(swap_endianness(static_cast<std::int32_t>(0x12345678)) == 0x78563412);
    
    template <typename T> constexpr T network_to_host(T value) {
        static_assert(std::is_integral<T>::value);
        
        // If we're big endian or it's one byte, pass as-is
        if(std::endian::native == std::endian::big) {
            return value;
        }
        
        // If we're little endian, swap
        else if(std::endian::little == std::endian::native) {
            return swap_endianness(value);
        }
        
        // If we're neither, die
        else {
            static_assert(std::endian::native == std::endian::big || std::endian::native == std::endian::little);
            std::terminate();
        }
    }
    
    template <typename T> constexpr T host_to_network(T value) {
        static_assert(std::is_integral<T>::value);
        
        // If we're big endian or it's one byte, pass as-is
        if(std::endian::native == std::endian::big) {
            return value;
        }
        
        // If we're little endian, swap
        else if(std::endian::native == std::endian::little) {
            return swap_endianness(value);
        }
        
        // If we're neither, die
        else {
            static_assert(std::endian::native == std::endian::big || std::endian::native == std::endian::little);
            std::terminate();
        }
    }
}

#endif
