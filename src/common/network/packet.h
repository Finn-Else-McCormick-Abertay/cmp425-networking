#pragma once

#include <prelude.h>
#include <prelude/opt.h>
#include <prelude/containers.h>
#include <SFML/Network/Packet.hpp>
#include <network/network_id.h>

class packet_id {
public:
    packet_id();
    packet_id(const str&);
    packet_id(const str& type, dyn_arr<str>&& args);
    packet_id(packet_id&&);
    packet_id(const packet_id&);

    const str& type() const;
    const dyn_arr<str>& args() const;

    opt<str> get_arg(size_t index) const;
    bool has_flag(const str& flag) const;

    const str& to_str() const;
    static str to_str(const packet_id&);
    static packet_id from_str(const str&);

    packet_id& operator=(const packet_id&);
    packet_id& operator=(packet_id&&);
private:
    str _type; dyn_arr<str> _args;
    mutable opt<str> _str_repr;
    
};

constexpr packet_id const operator ""_packid(const char* literal, size_t) { return packet_id(literal); }
static const packet_id packet_nullid = ""_packid;

bool operator==(const packet_id&, const packet_id&);
bool operator!=(const packet_id&, const packet_id&);
strong_ordering operator<=>(const packet_id&, const packet_id&);
template <> struct std::hash<packet_id> { size_t operator()(const packet_id& id) const { return std::hash<str_view>{}(id.to_str()); } };
inline auto format_as(const packet_id& id) { return id.to_str(); }


struct LogicalPacket {
    enum class MessageType { Default, Request, Lifecycle } type;
    network_id owner; packet_id id;
    uint64 time;
    sf::Packet contents;
    
    LogicalPacket();
    LogicalPacket(LogicalPacket&&) = default;
    LogicalPacket(const LogicalPacket&) = default;

    LogicalPacket& operator=(LogicalPacket&&) = default;
    LogicalPacket& operator=(const LogicalPacket&) = default;

    // If no time is provided, the current fixed tick at the moment of construction will be used.
    explicit LogicalPacket(const network_id&, const packet_id&,                    const opt<uint64>& time = nullopt, MessageType type = MessageType::Default);
    explicit LogicalPacket(const network_id&, const packet_id&, sf::Packet&&,      const opt<uint64>& time = nullopt, MessageType type = MessageType::Default);
    explicit LogicalPacket(const network_id&, const packet_id&, const sf::Packet&, const opt<uint64>& time = nullopt, MessageType type = MessageType::Default);

    static sf::Packet pack(const LogicalPacket&); void pack_into(sf::Packet&) const;
    static LogicalPacket unpack(sf::Packet&);
    
};