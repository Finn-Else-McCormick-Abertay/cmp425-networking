#include "packet.h"

#include <alias/ranges.h>

packet_id::packet_id(const str& type, dyn_arr<str>&& args) : _type(type), _args(move(args)), _str_repr(packet_id::to_str(*this)) {}
packet_id::packet_id(const str& s) : packet_id(move(from_str(s))) {}

packet_id::packet_id() : _type(""), _args({}), _str_repr("") {}
packet_id::packet_id(const packet_id& rhs) : _type(rhs._type), _args(rhs._args), _str_repr(rhs._str_repr) {}
packet_id::packet_id(packet_id&& rhs) : _type(rhs._type), _args(rhs._args), _str_repr(move(rhs._str_repr)) {}

packet_id& packet_id::operator=(const packet_id& rhs) { _type = rhs._type; _args = rhs._args; _str_repr = rhs._str_repr; return *this; }
packet_id& packet_id::operator=(packet_id&& rhs) { _type = move(rhs._type); _args = move(rhs._args); _str_repr = move(rhs._str_repr); return *this; }

const str& packet_id::type() const { return _type; }
const dyn_arr<str>& packet_id::args() const { return _args; }

opt<str> packet_id::get_arg(size_t index) const {
    if (index > _args.size()) return nullopt;
    return _args[index];
}

bool packet_id::has_flag(const str& flag) const {
    for (auto& arg : _args) { if (arg == flag) return true; }
    return false;
}

const str& packet_id::as_str() const {
    if (!_str_repr) _str_repr = packet_id::to_str(*this);
    return _str_repr.value();
}

str packet_id::to_str(const packet_id& id) {
    if (id._args.empty()) return id._type;
    return fmt::format("{}!{}", id._type, fmt::join(id._args, ";"));
}

packet_id packet_id::from_str(const str& s) {
    auto meta_start = s.find('!');
    if (meta_start == str::npos) return packet_id(s, {});
    str type = s.substr(0, meta_start);
    dyn_arr<str> args; for (auto arg : s.substr(meta_start + 1) | views::split(';') | ranges::to<dyn_arr<str>>()) args.emplace_back(arg);
    return packet_id(type, move(args));
}

bool operator==(const packet_id& lhs, const packet_id& rhs) { return lhs.as_str() == rhs.as_str(); }
bool operator!=(const packet_id& lhs, const packet_id& rhs) { return !(lhs == rhs); }
strong_ordering operator<=>(const packet_id& lhs, const packet_id& rhs) { return lhs.as_str() <=> rhs.as_str(); }

LogicalPacket::LogicalPacket(const packet_id& id, uint64 time, sf::Packet&& packet) : id(id), time(time), packet(move(packet)) {}
LogicalPacket::LogicalPacket(packet_id&& id, uint64 time, sf::Packet&& packet) : id(move(id)), time(time), packet(move(packet)) {}
LogicalPacket::LogicalPacket(const str& id, uint64 time, sf::Packet&& packet) : LogicalPacket(packet_id(id), time, move(packet)) {}
LogicalPacket::LogicalPacket() : id(), time(0), packet() {}