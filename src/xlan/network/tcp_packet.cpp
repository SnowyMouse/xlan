// SPDX-License-Identifier: GPL-3.0-only

#include <algorithm>

#include "tcp_packet.hpp"

namespace XLAN::Network {
    bool ConnectionInformation::verify(const char *password) const noexcept {
        return true; // TODO: verify bcrypt thingy
    }

    void ConnectionInformation::set_password(const char *password) noexcept {
        std::fill(this->password, this->password + sizeof(this->password), 0);
        // TODO: do the thing
    }
}
