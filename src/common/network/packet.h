#pragma once

#include <prelude.h>
#include <prelude/opt.h>
#include <prelude/containers.h>
#include <SFML/Network/Packet.hpp>

class PacketId {
public:
    PacketId(const str&); PacketId(); PacketId(const PacketId&); PacketId(PacketId&&);
    const str type; const dyn_arr<str> args;
    const str& as_str() const;
private:
    PacketId(const str& type, dyn_arr<str>&& args);
    mutable opt<str> _str_repr;
    
    static str to_str(const PacketId&);
    static PacketId from_str(const str&);
};

bool operator==(const PacketId&, const PacketId&);
bool operator!=(const PacketId&, const PacketId&);
strong_ordering operator<=>(const PacketId&, const PacketId&);
template <> struct std::hash<PacketId> { size_t operator()(const PacketId& id) const { return std::hash<str_view>{}(id.as_str()); } };

struct LogicalPacket {
    PacketId id;
    sf::Packet packet;
    
    LogicalPacket();
    LogicalPacket(const PacketId&, sf::Packet&& = sf::Packet());
    LogicalPacket(PacketId&&, sf::Packet&& = sf::Packet());
    LogicalPacket(const str&, sf::Packet&& = sf::Packet());
};