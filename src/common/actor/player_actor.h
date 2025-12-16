#pragma once

#include <actor/actor.h>
#include <network/network_id.h>

class ActorManager;

class PlayerActor : public INetworkedActor {
public:
    PlayerActor(PlayerActor&&);

private:
    str _player_ident;
    PlayerActor(const str& ident); friend class ActorManager;
    
    static constexpr frect2 DEFAULT_RECT = { { -5, -20 }, { 10, 20 } };
};