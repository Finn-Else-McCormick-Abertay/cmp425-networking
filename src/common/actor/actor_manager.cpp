#include "actor_manager.h"

DEFINE_SINGLETON(ActorManager);

void ActorManager::Registry::__register(IActor& actor) {
    inst()._known_actors.insert(&actor);
}
void ActorManager::Registry::__unregister(IActor& actor) {
    inst()._known_actors.erase(&actor);
}

PlayerActor& ActorManager::register_player(const str& ident) {
    auto [it, is_new] = inst()._players.try_emplace(ident, move(PlayerActor(ident)));
    if (!is_new) print<warning, ActorManager>("register_player called for previously registered player '{}'.", ident);
    else print<info, ActorManager>("Registered player '{}'.", ident);
    return it->second;
}

void ActorManager::unregister_player(const str& ident) {
    if (!inst()._players.erase(ident)) return print<warning, ActorManager>("unregister_player called for nonexistent player '{}'.", ident);
    print<info, ActorManager>("Unregistered player '{}'.", ident);
}

opt_ref<PlayerActor> ActorManager::get_player_actor(const str& ident) {
    if (!inst()._players.contains(ident)) return nullopt;
    return ref(inst()._players.at(ident));
}