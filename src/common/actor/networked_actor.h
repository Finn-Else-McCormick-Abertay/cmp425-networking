#pragma once

#include <actor/actor.h>
#include <network/networked.h>
#include <util/helper/enum_serialization.h>

namespace actor { enum class NetworkMode { NONE, AUTHORITY, LISTENER, RELAY }; }
ENUM_FORMATTER(actor::NetworkMode, (NONE, AUTHORITY, LISTENER, RELAY));

class ActorManager;

class INetworkedActor : public IActor, public INetworked {
public:
    actor::NetworkMode network_mode() const; void set_network_mode(actor::NetworkMode);
    bool is_authority() const;
    bool is_relay() const;

protected:
    actor::NetworkMode _network_mode;
    
    struct MotionInfo { fvec2 position; fvec2 velocity; fvec2 acceleration; };
    //bstmap<uint64, MotionInfo> _recieved_motion_info; friend class ActorManager;

    INetworkedActor(const network_id&, const frect2& rect = frect2(), actor::PhysicsMode = actor::PhysicsMode::DYNAMIC, actor::NetworkMode = actor::NetworkMode::NONE);
    INetworkedActor(INetworkedActor&&);
    virtual ~INetworkedActor();

    virtual dyn_arr<LogicalPacket> get_outstanding_messages() override;
    virtual result<LogicalPacket, str> get_requested_message(const packet_id& id) const override;
    virtual result<success_t, str> read_message(LogicalPacket&&) override;
};
