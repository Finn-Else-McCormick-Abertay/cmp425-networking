#pragma once

#include <prelude.h>
#include <prelude/opt.h>
#include <prelude/containers.h>
#include <util/helper/singleton.h>
#include <actor/actor.h>
#include <actor/player_actor.h>

class ActorManager { DECL_SINGLETON(ActorManager);
public:
    DECL_REGISTRY(IActor);

    static PlayerActor& register_player(const str& ident);
    static void unregister_player(const str& ident);
    static opt_ref<PlayerActor> get_player_actor(const str& ident);

private:
    set<IActor*> _known_actors;
    hashmap<str, PlayerActor> _players;
};