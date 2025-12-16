#pragma once

#include <prelude.h>
#include <prelude/containers.h>
#include <prelude/opt.h>

#include <network/packet.h>
#include <network/network_id.h>
#include <util/construct_markers.h>

class INetworked {
public:
    explicit INetworked(construct_noinit_t);
    INetworked(INetworked&&);
    virtual ~INetworked();

    const network_id& network_id() const;

    dyn_arr<LogicalPacket> outstanding();
    opt<LogicalPacket> request(const packet_id& id) const;
    void read(LogicalPacket&&);

protected:
    INetworked(const ::network_id& network_id);
    void set_network_id(const ::network_id&);

    virtual dyn_arr<LogicalPacket> get_outstanding_messages();
    virtual result<LogicalPacket, str> get_requested_message(const packet_id& id) const;
    virtual result<success_t, str> read_message(LogicalPacket&&);

private:
    ::network_id _network_id; bool _invalidated = false;
};