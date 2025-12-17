#include "address.h"

#include <prelude/containers.h>
#include <alias/ranges.h>

SocketAddress::SocketAddress(const IpAddress& ip, Port port) : ip(ip), port(port) {}

bool SocketAddress::operator==(const SocketAddress& rhs) const { return ip == rhs.ip && port == rhs.port; }

strong_ordering SocketAddress::operator<=>(const SocketAddress& rhs) const {
    if (ip < rhs.ip) return strong_ordering::less;
    if (ip > rhs.ip) return strong_ordering::greater;
    return port <=> rhs.port;
}

str SocketAddress::to_str() const { return fmt::format("{}", *this); }
opt<SocketAddress> SocketAddress::resolve(const str& val) {
    auto split = views::split(val, ':') | ranges::to<dyn_arr<str>>();
    if (split.size() >= 2 || split.empty()) return nullopt;

    auto ip_opt = IpAddress::resolve(split.at(0));
    if (!ip_opt) return nullopt;

    uint32 port = Socket::AnyPort;
    if (split.size() == 2) port = str_to<uint32>(split.at(1));

    return SocketAddress(*ip_opt, port);
}