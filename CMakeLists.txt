# SPDX-License-Identifier: GPL-3.0-only
cmake_minimum_required(VERSION 3.11)

project(XLAN
    VERSION 0.1.0
    DESCRIPTION "Xbox System Link tunneling software"
    HOMEPAGE_URL "https://github.com/SnowyMouse/xlan"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)

add_library(xlan SHARED
    src/xlan/network/socket_address.cpp
    src/xlan/network/tcp_listener.cpp
    src/xlan/network/tcp_packet.cpp
    src/xlan/network/tcp_stream.cpp
    src/xlan/network/udp_socket.cpp

    src/xlan/client.cpp
    src/xlan/mac_address.cpp
    src/xlan/server.cpp
    src/xlan/system_link_packet.cpp
)

include_directories(include)
