#pragma once

#include <prelude.h>
#include <prelude/opt.h>
#include <prelude/containers.h>
#include <util/helper/singleton.h>
#include <actor/actor.h>
#include <actor/networked_actor.h>
#include <actor/player_actor.h>
#include <system/system.h>
#include <util/helper/glaze_enum_helper.h>

namespace actor { enum class InterpolationMode { DEFAULT, NONE, LINEAR }; }
ENUM_GLAZE_AND_FORMATTER(actor::InterpolationMode, (DEFAULT, NONE, LINEAR));

class ActorManager : INetworked, IFixedTickingSystem { DECL_SINGLETON_WITH_CONSTRUCTOR(ActorManager);
public:
    DECL_MULTI_REGISTRY(IActor, INetworkedActor);

    static void init();
    
    static actor::InterpolationMode interpolation_mode();
    static void set_interpolation_mode(actor::InterpolationMode);

    static constexpr actor::InterpolationMode CLIENT_DEFAULT_INTERPOLATION = actor::InterpolationMode::LINEAR;
    static constexpr actor::InterpolationMode SERVER_DEFAULT_INTERPOLATION = actor::InterpolationMode::NONE;

    static PlayerActor& register_player(const str& ident, bool broadcast = true, bool fail_quiet = false);
    static void unregister_player(const str& ident, bool broadcast = true, bool fail_quiet = false);
    static opt_ref<PlayerActor> get_player_actor(const str& ident);

    static void update_player_authority_states();
    
    virtual void fixed_tick() override;
    
    static void perform_physics_step(IActor& actor);
    static void handle_collisions(IActor& actor, bool apply_friction = true);

    static dyn_arr<str> debug_message();

private:
    set<IActor*> _known_actors;
    set<INetworkedActor*> _known_networked_actors;

    actor::InterpolationMode _interpolation_mode;

    hashmap<str, PlayerActor> _players;
    
    virtual result<LogicalPacket, str> get_requested_message(const packet_id& id) const override;
    virtual result<success_t, str> read_message(LogicalPacket&&) override;
};