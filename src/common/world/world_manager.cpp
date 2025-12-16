#include "world_manager.h"

#include <save/save_manager.h>
#include <network/network_manager.h>

DEFINE_SINGLETON(WorldManager);

WorldManager::WorldManager() : INetworked(::network_id("world_manager"_id, "")) {}

opt_ref<World> WorldManager::world() {
    if (inst()._world) return *inst()._world;
    return nullopt;
}

opt_ref<Level> WorldManager::level(const id& level_id) {
    if (auto world_opt = world()) {
        World& world = *world_opt;
        return world.level(level_id);
    }
    return nullopt;
}

void WorldManager::init() {
    inst();
    /*if (!inst()._world && NetworkManager::server_address()) {
        // Request world information from server
        NetworkManager::request(inst().network_id(), packet_id("world"), *NetworkManager::server_address());
    }*/
}

bool WorldManager::try_save() {
    if (!inst()._world || !inst()._world.value().is_authority()) return false;

    auto result = SaveManager::save_world(*inst()._world);
    if (!result) {
        print<error, WorldManager>("Failed to save world '{}': {}", inst()._world->name(), result.error());
        return false;
    }
    print<success, WorldManager>("Saved world '{}'.", inst()._world->name());
    return true;
}

void WorldManager::internal_load(World&& world, bool authority) {
    inst()._world.emplace(move(world));
    inst()._world->_authority = authority;
    inst()._world->network_reregister();

    if (auto default_level = inst()._world->level("world"_id); !default_level) inst()._world->make_level("world"_id);

    print<success, WorldManager>("Loaded world '{}' as {}authority.", inst()._world->name(), authority ? "" : "non-");
    if (authority) NetworkManager::broadcast(inst().network_id(), packet_id("world"));
}

bool WorldManager::load_from_file(const str& name) {
    auto result = SaveManager::load_world(name);
    if (!result) {
        print<error, WorldManager>("Failed to load world '{}': {}", name, result.error());
        return false;
    }
    
    auto write_result = glz::write_json(*result);
    if (write_result) print<debug, WorldManager>(*write_result);

    internal_load(move(*result), true);
    return true;
}

bool WorldManager::create(const str& name, bool replace) {
    if (inst()._world && !replace) return false;
    print<success, WorldManager>("Created world '{}'.", name);
    internal_load(World(name), true);
    return true;
}

result<LogicalPacket, str> WorldManager::get_requested_message(const packet_id& id) const {
    if (id.type() == "world") {
        if (id.args().size() > 0) return err("Too many args.");
        //bool full = id.args().size() == 1 ? id.args()[0] == "full" : false;
        
        if (!_world) return err("No loaded world.");
        
        auto result = glz::write_json(*_world);
        if (!result) return err("Serialisation failed.");

        auto packet = LogicalPacket(id);
        packet.packet << *result;
        return move(packet);
    }
    return err("");
}

result<success_t, str> WorldManager::read_message(LogicalPacket&& packet) {
    if (packet.id.type() == "world") {
        if (packet.id.args().size() > 0) return err("Too many args.");
        if (_world && _world->is_authority()) return err("Authoritative world is already loaded.");
        
        //bool full = packet.id.args().size() == 1 ? packet.id.args()[0] == "full" : false;

        str buffer; packet.packet >> buffer;
        auto result = glz::read_json<World>(buffer);
        if (!result) return err("Deserialisation failed.");

        print<debug, WorldManager>(buffer);
        
        internal_load(move(*result), false);
        return empty_success;
    }
    return err("");
}