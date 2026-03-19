#pragma once

#include <actor/actor.h>
#include <network/network_id.h>

class ActorManager;

class PlayerActor : public INetworkedActor {
public:
    PlayerActor(PlayerActor&&);

    const str& ident() const;

    bool is_authority() const;
    void update_authority_state();

private:
    str _player_ident;
    bool _authority;
    PlayerActor(const str& ident); friend class ActorManager;
    
    static constexpr frect2 DEFAULT_RECT = { { -5, -20 }, { 10, 20 } };
    
    #ifdef CLIENT
    virtual void draw(sf::RenderTarget&, draw_layer layer) override;
    #endif
    
    virtual dyn_arr<LogicalPacket> get_outstanding_messages() override;
    virtual result<success_t, str> read_message(LogicalPacket&&) override;

    static constexpr float VALID_DIFF_EPSILON = 0.000001f;
    fvec2 _prev_sent_position; fvec2 _prev_sent_velocity; fvec2 _prev_sent_accel;
};