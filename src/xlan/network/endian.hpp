// SPDX-License-Identifier: GPL-3.0-only

#ifndef XLAN__NETWORK__ENDIAN_HPP
#define XLAN__NETWORK__ENDIAN_HPP

namespace XLAN::Network {
    template <typename T> constexpr T swap_endianness(T value) {
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
    
    template <typename T> struct NetworkEndian {
        std::byte data[sizeof(T)];
        
        static_assert(std::is_integral<T>::value);
        
        /** Get the stored value, automatically converting endianness if needed */
        operator T() const noexcept {
            if(std::endian::native == std::endian::big) {
                return *reinterpret_cast<const T *>(data);
            }
            else {
                return swap_endianness(*reinterpret_cast<const T *>(data));
            }
        }
        
        /** Set the stored value, automatically converting endianness if needed */
        T operator =(const T &what) noexcept {
            if(std::endian::native == std::endian::big) {
                *reinterpret_cast<T *>(data) = what;
            }
            else {
                *reinterpret_cast<T *>(data) = swap_endianness(what);
            }
            return what;
        }
    };
}

#endif
