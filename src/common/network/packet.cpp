#include "packet.h"

#include <alias/ranges.h>
#include <system/system_manager.h>

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

const str& packet_id::to_str() const {
    if (!_str_repr) _str_repr = packet_id::to_str(*this);
    return _str_repr.value();
}

str packet_id::to_str(const packet_id& id) {
    if (id._args.empty()) return id._type;
    return fmt::format("{}!{}", id._type, fmt::join(id._args, ";"));
}

packet_id packet_id::from_str(const str& s) {
    if (auto meta_start = s.find('!'); meta_start != str::npos)
        return packet_id(s.substr(0, meta_start), s.substr(meta_start + 1) | views::split(';') | ranges::to<dyn_arr<str>>());
    return packet_id(s, {});
}

bool operator==(const packet_id& lhs, const packet_id& rhs) { return lhs.to_str() == rhs.to_str(); }
bool operator!=(const packet_id& lhs, const packet_id& rhs) { return !(lhs == rhs); }
strong_ordering operator<=>(const packet_id& lhs, const packet_id& rhs) { return lhs.to_str() <=> rhs.to_str(); }


LogicalPacket::LogicalPacket(const network_id& netid, const packet_id& packid, const opt<uint64>& time, MessageType type)
    : owner(netid), id(packid), type(type), time(time.value_or(SystemManager::get_fixed_tick())), contents() {}

LogicalPacket::LogicalPacket(const network_id& netid, const packet_id& packid, sf::Packet&& contents, const opt<uint64>& time, MessageType type)
    : owner(netid), id(packid), type(type), time(time.value_or(SystemManager::get_fixed_tick())), contents(move(contents)) {}

LogicalPacket::LogicalPacket(const network_id& netid, const packet_id& packid, const sf::Packet& contents,  const opt<uint64>& time, MessageType type)
    : owner(netid), id(packid), type(type), time(time.value_or(SystemManager::get_fixed_tick())), contents(contents) {}

LogicalPacket::LogicalPacket() : LogicalPacket(network_nullid, packet_nullid) {}


sf::Packet LogicalPacket::pack(const LogicalPacket& unpacked) {
    sf::Packet result;
    unpacked.pack_into(result);
    return result;
}

void LogicalPacket::pack_into(sf::Packet& packed) const {
    packed << owner.to_str();
    packed << id.to_str();
    packed << (uint8)type;
    packed << (uint64)time;
    packed << (uint64)contents.getDataSize();
    packed.append(contents.getData(), contents.getDataSize());
}

LogicalPacket LogicalPacket::unpack(sf::Packet& packed) {
    str owner_str;      packed >> owner_str;
    str id_str;         packed >> id_str;
    uint8 type;         packed >> type;
    uint64 time;        packed >> time;
    uint64 data_size;   packed >> data_size;

    uint8 data[data_size];
    for (uint16 j = 0; j < data_size; ++j) packed >> data[j];

    auto result = LogicalPacket(network_id::from_str(owner_str), packet_id::from_str(id_str), time, (MessageType) type);
    result.contents.append(data, data_size);
    return result;
}