#pragma once

#include <prelude.h>
#include <prelude/opt.h>
#include <data/namespaced_id.h>

#include <SFML/Network/Packet.hpp>

enum class PacketPriority {
    SINGLE_SEND,
    REQUIRE_CONFIRMATION
};

struct LogicalPacket {
    str packet_id;
    PacketPriority priority;
    sf::Packet packet;

    LogicalPacket(str, PacketPriority, sf::Packet&&);
};

class INetworked {
public:
    INetworked();
    virtual ~INetworked();

    virtual str network_id() const = 0;

    virtual opt<LogicalPacket> write_message() const = 0;
    virtual opt<LogicalPacket> resend_message(str, uint64) const;

    virtual void read_message(LogicalPacket&&) = 0;
};