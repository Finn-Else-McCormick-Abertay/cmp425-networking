#pragma once

#include <prelude.h>
#include <prelude/opt.h>
#include <SFML/Network.hpp>
#include <fmt/format.h>
#include <alias/compare.h>
#include <alias/SFML/network.h>
#include <util/format/SFML/network.h>

using Port = uint16;

struct SocketAddress {
    IpAddress ip; Port port;

    SocketAddress(const SocketAddress&) = default; SocketAddress(SocketAddress&&) = default;
    SocketAddress(const IpAddress& ip = IpAddress::Any, Port port = Socket::AnyPort);

    SocketAddress& operator=(const SocketAddress&) = default;
    SocketAddress& operator=(SocketAddress&&) = default;

    bool operator==(const SocketAddress& rhs) const;
    strong_ordering operator<=>(const SocketAddress& rhs) const;

    str to_str() const;
    static opt<SocketAddress> resolve(const str&);
};

inline auto format_as(const SocketAddress& address) {
    if (address.port == Socket::AnyPort) return fmt::format("{}:Any", address.ip);
    return fmt::format("{}:{}", address.ip, address.port);
}

template<> struct std::hash<SocketAddress> {
    size_t operator()(const SocketAddress& address) const noexcept {
        auto hash = std::hash<uint32>{};
        return hash(address.ip.toInteger()) ^ (hash(address.port) << 1);
    }
};