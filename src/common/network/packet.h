#pragma once

#include <prelude.h>
#include <prelude/opt.h>
#include <prelude/containers.h>
#include <SFML/Network/Packet.hpp>

class packet_id {
public:
    packet_id(const str&); packet_id();
    packet_id(const str& type, dyn_arr<str>&& args);
    packet_id(const packet_id&); packet_id(packet_id&&);

    const str& type() const;
    const dyn_arr<str>& args() const;

    opt<str> get_arg(size_t index) const;
    bool has_flag(const str& flag) const;

    const str& as_str() const;

    packet_id& operator=(const packet_id&);
    packet_id& operator=(packet_id&&);
private:
    str _type; dyn_arr<str> _args;
    mutable opt<str> _str_repr;
    
    static str to_str(const packet_id&);
    static packet_id from_str(const str&);
};

bool operator==(const packet_id&, const packet_id&);
bool operator!=(const packet_id&, const packet_id&);
strong_ordering operator<=>(const packet_id&, const packet_id&);
template <> struct std::hash<packet_id> { size_t operator()(const packet_id& id) const { return std::hash<str_view>{}(id.as_str()); } };
inline auto format_as(const packet_id& id) { return id.as_str(); }

struct LogicalPacket {
    packet_id id;
    uint64 time;
    sf::Packet packet;
    
    LogicalPacket();
    explicit LogicalPacket(const packet_id&, uint64 time = 0, sf::Packet&& = sf::Packet());
    explicit LogicalPacket(packet_id&&, uint64 time = 0, sf::Packet&& = sf::Packet());
    explicit LogicalPacket(const str&, uint64 time = 0, sf::Packet&& = sf::Packet());

    LogicalPacket(LogicalPacket&&) = default;
    LogicalPacket(const LogicalPacket&) = default;

    LogicalPacket& operator=(LogicalPacket&&) = default;
    LogicalPacket& operator=(const LogicalPacket&) = default;
};