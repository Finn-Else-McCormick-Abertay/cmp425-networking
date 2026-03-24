#pragma once

#include <actor/networked_actor.h>

class ActorManager;

class PlayerActor : public INetworkedActor {
public:
    PlayerActor(PlayerActor&&);

    const str& ident() const;

private:
    str _player_ident;
    PlayerActor(const str& ident); friend class ActorManager;
    
    static constexpr frect2 DEFAULT_RECT = { { -5, -20 }, { 10, 20 } };
    
    #ifdef CLIENT
    virtual void draw(sf::RenderTarget&, draw_layer layer) override;
    #endif
};