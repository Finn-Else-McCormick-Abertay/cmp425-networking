#pragma once

#include <actor/networked_actor.h>

class PlayerActor : public INetworkedActor {
public:
    PlayerActor(const str& ident, actor::NetworkMode = actor::NetworkMode::AUTHORITY);
    PlayerActor(PlayerActor&&);

    const str& ident() const;

private:
    str _player_ident;
    
    #ifdef CLIENT
    virtual void draw(sf::RenderTarget&, draw_layer layer) override;
    #endif
};