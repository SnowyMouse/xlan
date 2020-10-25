// SPDX-License-Identifier: GPL-3.0-only

#ifndef XLAN__CLOCK_HPP
#define XLAN__CLOCK_HPP

#include <chrono>

namespace XLAN {
    /**
     * Clock is used for keeping track of time with a high degree of precision
     */
    using Clock = std::chrono::steady_clock;
}

#endif
