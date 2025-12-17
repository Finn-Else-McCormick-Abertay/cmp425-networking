#pragma once

#include <prelude.h>
#include <prelude/opt.h>
#include <prelude/containers.h>
#include <util/helper/singleton.h>
#include <actor/actor.h>
#include <actor/player_actor.h>
#include <network/networked.h>

class PhysicsSystem;

class ActorManager : INetworked { DECL_SINGLETON_WITH_CONSTRUCTOR(ActorManager);
public:
    DECL_REGISTRY(IActor);

    static void init();

    static PlayerActor& register_player(const str& ident, bool authority, bool broadcast = true, bool fail_quiet = false);
    static void unregister_player(const str& ident, bool broadcast = true, bool fail_quiet = false);
    static opt_ref<PlayerActor> get_player_actor(const str& ident);

    static void tick(float delta);

private:
    set<IActor*> _known_actors;
    hashmap<str, PlayerActor> _players;
    
    friend class PhysicsSystem;
    
    virtual result<LogicalPacket, str> get_requested_message(const packet_id& id) const override;
    virtual result<success_t, str> read_message(LogicalPacket&&) override;
};