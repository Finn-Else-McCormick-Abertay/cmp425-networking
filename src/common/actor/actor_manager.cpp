#include "actor_manager.h"

#include <network/network_manager.h>

DEFINE_SINGLETON(ActorManager);

ActorManager::ActorManager() : INetworked(::network_id("singleton"_id, "actor_manager")) {}

void ActorManager::Registry::__register(IActor& actor) {
    inst()._known_actors.insert(&actor);
}
void ActorManager::Registry::__unregister(IActor& actor) {
    inst()._known_actors.erase(&actor);
}

void ActorManager::init() {
    // This is necessary cause of the weird pattern I've locked myself into where static initialisation order is something I have to consider
    inst();
}

PlayerActor& ActorManager::register_player(const str& ident, bool broadcast) {
    auto [it, is_new] = inst()._players.try_emplace(ident, move(PlayerActor(ident)));
    if (is_new && broadcast) NetworkManager::broadcast(inst().network_id(), packet_id("player", { "connected", ident }));

    if (!is_new) print<warning, ActorManager>("register_player called for previously registered player '{}'.", ident);
    else print<info, ActorManager>("Registered player '{}'.", ident);
    return it->second;
}

void ActorManager::unregister_player(const str& ident, bool broadcast) {
    if (!inst()._players.erase(ident)) return print<warning, ActorManager>("unregister_player called for nonexistent player '{}'.", ident);
    if (broadcast) NetworkManager::broadcast(inst().network_id(), packet_id("player", { "disconnected", ident }));
    print<info, ActorManager>("Unregistered player '{}'.", ident);
}

opt_ref<PlayerActor> ActorManager::get_player_actor(const str& ident) {
    if (!inst()._players.contains(ident)) return nullopt;
    return ref(inst()._players.at(ident));
}

result<LogicalPacket, str> ActorManager::get_requested_message(const packet_id& id) const {
    // This shouldn't really be necessary for an id-only message, but I don't have time to refactor this now
    if (id.type() == "player") {
        auto event_arg = id.get_arg(0); auto ident_arg = id.get_arg(1);
        if (!event_arg) return err("Missing event arg."); else if (!ident_arg) return err("Missing player ident arg.");
        auto& event = *event_arg; auto& ident = *ident_arg;
        
        if (event == "connected") return LogicalPacket(id);
        else if (event == "disconnected") return LogicalPacket(id);
        else return err("Invalid event arg. Must be one of 'connected' or 'disconnected'.");
    }
    return err("Unknown packet type.");
}

result<success_t, str> ActorManager::read_message(LogicalPacket&& packet) {
    if (packet.id.type() == "player") {
        auto event_arg = packet.id.get_arg(0); auto ident_arg = packet.id.get_arg(1);
        if (!event_arg) return err("Missing event arg."); else if (!ident_arg) return err("Missing player ident arg.");
        auto& event = *event_arg; auto& ident = *ident_arg;
        
        if (event == "connected") {
            register_player(ident, false);
            return empty_success;
        }
        else if (event == "disconnected") {
            unregister_player(ident, false);
            return empty_success;
        }
        else return err("Invalid event arg. Must be one of 'connected' or 'disconnected'.");
    }
    return err("Unknown packet type.");
}