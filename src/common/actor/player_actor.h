#pragma once

#include <actor/networked_actor.h>

class PlayerActor : public INetworkedActor {
public:
    PlayerActor(const str& ident, const str& display_name, actor::NetworkMode = actor::NetworkMode::NONE);
    PlayerActor(PlayerActor&&);

    const str& ident() const;
    const str& display_name() const;

private:
    str _player_ident;
    str _player_display_name;
    
    #ifdef CLIENT
    virtual void draw(sf::RenderTarget&, draw_layer layer) override;
    #endif
};