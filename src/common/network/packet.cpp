#include "packet.h"

#include <alias/ranges.h>

PacketId::PacketId(const str& type, dyn_arr<str>&& args) : type(type), args(move(args)), _str_repr(PacketId::to_str(*this)) {}
PacketId::PacketId(const str& s) : PacketId(move(from_str(s))) {}

PacketId::PacketId() : type(""), args({}), _str_repr("") {}
PacketId::PacketId(const PacketId& rhs) : type(rhs.type), args(rhs.args), _str_repr(rhs._str_repr) {}
PacketId::PacketId(PacketId&& rhs) : type(rhs.type), args(rhs.args), _str_repr(move(rhs._str_repr)) {}

const str& PacketId::as_str() const {
    if (!_str_repr) _str_repr = PacketId::to_str(*this);
    return _str_repr.value();
}

str PacketId::to_str(const PacketId& id) {
    if (id.args.empty()) return id.type;
    return fmt::format("{}!{}", id.type, fmt::join(id.args, ";"));
}

PacketId PacketId::from_str(const str& s) {
    auto meta_start = s.find('!');
    if (meta_start == str::npos) return PacketId(s, {});
    str type = s.substr(0, meta_start);
    dyn_arr<str> args; for (auto arg : s.substr(meta_start) | views::split(';') | ranges::to<dyn_arr<str>>()) args.emplace_back(arg);
    return PacketId(type, move(args));
}

bool operator==(const PacketId& lhs, const PacketId& rhs) { return lhs.as_str() == rhs.as_str(); }
bool operator!=(const PacketId& lhs, const PacketId& rhs) { return !(lhs == rhs); }
strong_ordering operator<=>(const PacketId& lhs, const PacketId& rhs) { return lhs.as_str() <=> rhs.as_str(); }

LogicalPacket::LogicalPacket(const PacketId& id, sf::Packet&&) : id(id), packet(move(packet)) {}
LogicalPacket::LogicalPacket(PacketId&& id, sf::Packet&& packet) : id(move(id)), packet(move(packet)) {}
LogicalPacket::LogicalPacket(const str&, sf::Packet&& packet) : LogicalPacket(PacketId(id), move(packet)) {}
LogicalPacket::LogicalPacket() : id(), packet() {}