#pragma once

#include <prelude.h>
#include <prelude/containers.h>
#include <data/namespaced_id.h>

#include <SFML/Network/Packet.hpp>

struct LogicalPacket {
    str packet_id;
    sf::Packet packet;
    
    LogicalPacket(str, sf::Packet&&);
};

class INetworked {
public:
    INetworked(str network_id);
    INetworked(INetworked&&);
    virtual ~INetworked();

    str network_id() const;

    virtual dyn_arr<LogicalPacket> write_messages() const = 0;
    virtual void read_message(LogicalPacket&&) = 0;

protected:
    void set_network_id(const str&);

private:
    str _network_id; bool _invalidated = false;
};