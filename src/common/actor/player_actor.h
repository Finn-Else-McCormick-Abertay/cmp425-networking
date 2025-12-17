#pragma once

#include <actor/actor.h>
#include <network/network_id.h>

class ActorManager;

class PlayerActor : public INetworkedActor {
public:
    PlayerActor(PlayerActor&&);

private:
    str _player_ident;
    PlayerActor(const str& ident, bool listen_only); friend class ActorManager;
    
    static constexpr frect2 DEFAULT_RECT = { { -5, -20 }, { 10, 20 } };
    
    #ifdef CLIENT
    virtual void draw(sf::RenderTarget&, draw_layer layer) override;
    #endif
    
    virtual dyn_arr<LogicalPacket> get_outstanding_messages() override;
    virtual result<success_t, str> read_message(LogicalPacket&&) override;

    bool _listen_only;
};