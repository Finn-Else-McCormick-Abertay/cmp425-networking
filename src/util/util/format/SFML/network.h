#pragma once

#include <alias/str.h>
#include <SFML/Network.hpp>
#include <fmt/format.h>
#include <util/helper/enum_serialization.h>

// Socket status formatter
ENUM_FORMATTER(sf::Socket::Status, (Done, NotReady, Partial, Disconnected, Error), { .use_type_name = false });

// IP address formatter
template <> struct fmt::formatter<sf::IpAddress> : formatter<str_view> {
    inline auto format(const sf::IpAddress& address, format_context& ctx) const {
        if (address == sf::IpAddress::LocalHost) return formatter<str_view>().format("LocalHost", ctx);
        if (address == sf::IpAddress::Broadcast) return formatter<str_view>().format("Broadcast", ctx);
        if (address == sf::IpAddress::Any) return formatter<str_view>().format("Any", ctx);
        return formatter<str>().format(address.toString(), ctx);
    }
};