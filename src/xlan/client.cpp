// SPDX-License-Identifier: GPL-3.0-only

#include <xlan/server.hpp>
#include <xlan/client.hpp>

namespace XLAN {
    std::optional<std::uint32_t> Client::get_ping() const noexcept {
        if(this->ping_count == 0) {
            return std::nullopt; // we haven't been pinged once yet
        }
        std::uint64_t sum = 0;
        for(std::size_t i = 0; i < ping_count; i++) {
            sum += this->pings[i];
        }
        return sum / ping_count;
    }
    
    void Client::drop(const char *reason) {
        std::terminate(); // TODO
    }
    
    void Client::set_op(bool op, const char *reason) {
        std::terminate(); // TODO
    }
    
    void Client::message(const char *message) const {
        std::terminate(); // TODO
    }
}
