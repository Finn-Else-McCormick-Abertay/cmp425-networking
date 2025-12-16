#include "actor_manager.h"

DEFINE_SINGLETON(ActorManager);

void ActorManager::Registry::__register(IActor& actor) {
    inst()._known_actors.insert(&actor);
}
void ActorManager::Registry::__unregister(IActor& actor) {
    inst()._known_actors.erase(&actor);
}

PlayerActor& ActorManager::register_player(const str& ident) {
    if (auto opt = get_player_actor(ident)) {
        print<warning, ActorManager>("register_player called for previously registered player '{}'.", ident);
        return *opt;
    }
    inst()._players.emplace(ident, move(PlayerActor(ident)));
    return inst()._players.at(ident);
}

void ActorManager::unregister_player(const str& ident) {
    if (!inst()._players.erase(ident)) print<warning, ActorManager>("unregister_player called for nonexistent player '{}'.", ident);
}

opt_ref<PlayerActor> ActorManager::get_player_actor(const str& ident) {
    if (!inst()._players.contains(ident)) return nullopt;
    return ref(inst()._players.at(ident));
}