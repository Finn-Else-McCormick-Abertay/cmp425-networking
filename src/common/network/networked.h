#pragma once

#include <prelude.h>
#include <prelude/containers.h>
#include <prelude/opt.h>

#include <network/packet.h>

class INetworked {
public:
    INetworked(str network_id);
    INetworked(INetworked&&);
    virtual ~INetworked();

    str network_id() const;

    dyn_arr<LogicalPacket> outstanding() const;
    opt<LogicalPacket> request(const PacketId& id) const;
    void read(LogicalPacket&&);

protected:
    virtual dyn_arr<LogicalPacket> get_outstanding_messages() const;
    virtual result<LogicalPacket, str> get_requested_message(const PacketId& id) const;
    virtual result<none_t, str> read_message(LogicalPacket&&) = 0;

    void set_network_id(const str&);

private:
    str _network_id; bool _invalidated = false;
};