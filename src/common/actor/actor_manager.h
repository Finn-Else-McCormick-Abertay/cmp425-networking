#pragma once

#include <prelude.h>
#include <prelude/opt.h>
#include <prelude/containers.h>
#include <util/helper/singleton.h>
#include <actor/actor.h>
#include <actor/networked_actor.h>
#include <actor/player_actor.h>
#include <system/system.h>
#include <actor/interpolation.h>

class ActorManager : INetworked, IFixedTickingSystem { DECL_SINGLETON_WITH_CONSTRUCTOR(ActorManager);
public:
    DECL_MULTI_REGISTRY(IActor, INetworkedActor);

    /** Initialise manager. */
    static void init();
    
    /** Get messages to be displayed in the debug hud this frame. */
    static dyn_arr<str> debug_message();
    

    /* ------------------------- */
    /* -- Actor Interpolation -- */
    /* ------------------------- */
    
    static Interpolation interpolation();
    static void set_interpolation(Interpolation);

    
    /* ------------------------- */
    /* --    Actor Physics    -- */
    /* ------------------------- */
    
    virtual void fixed_tick() override;
    
    static void perform_physics_step(IActor& actor);
    static void move_actor_respecting_collision(IActor& actor, const fvec2& motion);
    static void handle_collisions(IActor& actor, bool apply_friction = true);
    

    /* ------------------------- */
    /* -- Player Registration -- */
    /* ------------------------- */

    static PlayerActor& register_player(const str& ident, const str& display_name, bool broadcast = true, bool fail_quiet = false);
    static void unregister_player(const str& ident, bool broadcast = true, bool fail_quiet = false);
    static opt_ref<PlayerActor> get_player_actor(const str& ident);

    static void update_player_authority_states();

private:
    Interpolation _interpolation;

    bstmap<IActor*, INetworkedActor*> _known_actors;
    // This has to be declared after _known_actors or the program will crash while exiting. Singletons were a mistake.
    hashmap<str, PlayerActor> _players;
    
    virtual result<LogicalPacket, str> get_requested_message(const packet_id& id) const override;
    virtual result<success_t, str> read_message(LogicalPacket&&) override;
};