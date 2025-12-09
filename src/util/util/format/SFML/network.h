#pragma once

#include <SFML/Network.hpp>
#include <util/helper/enum_serialization.h>

// Socket status formatter
ENUM_FORMATTER(sf::Socket::Status, (
        Done,
        NotReady,
        Partial,
        Disconnected,
        Error
    ),
    use_type_name = false
);

// IP address formatter
template <> struct fmt::formatter<sf::IpAddress>: formatter<string_view> {
    inline auto format(const sf::IpAddress& address, format_context& ctx) const {
        if (address == sf::IpAddress::LocalHost) return formatter<str>().format("LocalHost", ctx);
        return formatter<str>().format(address.toString(), ctx);
    }
};