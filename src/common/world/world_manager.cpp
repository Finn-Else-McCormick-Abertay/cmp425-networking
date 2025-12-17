#include "world_manager.h"

#include <save/save_manager.h>
#include <network/network_manager.h>
#include <regex>

DEFINE_SINGLETON(WorldManager);

WorldManager::WorldManager() : INetworked(::network_id("singleton"_id, "world_manager")) {}

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
    #ifdef CLIENT
    if (!inst()._world && NetworkManager::server_address()) {
        // Request world information from server
        NetworkManager::request(inst().network_id(), packet_id("world", { "joined" }), *NetworkManager::server_address());
    }
    #elifdef SERVER
    // If a world name was not provided by the cli (runs before the init methods)
    if (!inst()._world) {
        // Load fallback world
        if (!load_from_file("fallback_world")) create("fallback_world");
    }
    #endif
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
    if (authority) NetworkManager::broadcast(inst().network_id(), packet_id("world!loaded"));
}

void WorldManager::internal_unload() {
    if (!inst()._world) return;

    auto name = inst()._world->name();
    auto authority = inst()._world->is_authority();

    inst()._world = nullopt;
    print<success, WorldManager>("Unloaded world '{}'.", name);
    if (authority) NetworkManager::broadcast(inst().network_id(), packet_id("world!unloaded"));
}

opt<str> WorldManager::validate_name(const str& name) {
    if (name.empty()) return nullopt;
    return std::regex_replace(name,std::regex("[\\\\\\/?+]"), "_");
}

bool WorldManager::load_from_file(const str& name) {
    auto name_opt = validate_name(name);
    if (!name_opt) return false;
    str name_validated = *name_opt;
    auto result = SaveManager::load_world(name_validated);
    if (!result) {
        print<error, WorldManager>("Failed to load world '{}': {}", name_validated, result.error());
        return false;
    }
    internal_load(move(*result), true);
    return true;
}

bool WorldManager::create(const str& name, bool replace) {
    if (inst()._world && !replace) return false;
    str name_validated = *validate_name(name);
    print<success, WorldManager>("Created world '{}'.", name_validated);
    internal_load(World(name_validated), true);
    return true;
}

result<LogicalPacket, str> WorldManager::get_requested_message(const packet_id& id) const {
    if (id.type() == "world") {
        auto event_arg = id.get_arg(0);
        if (!event_arg) return err("No event arg provided. Must be one of 'loaded', 'unloaded' or 'joined'.");
        auto& event = *event_arg;

        if (event == "loaded" || event == "joined") {
            if (!_world) return err("No loaded world.");
            
            auto result = glz::write_json(*_world);
            if (!result) return err("Serialisation failed.");

            auto packet = LogicalPacket(id);
            packet.packet << *result;
            return move(packet);
        }
        else if (event == "unloaded") {
            return LogicalPacket(id);
        }
        else return err(fmt::format("Invalid event arg '{}'.", event));
    }
    return err("Invalid packet type.");
}

result<success_t, str> WorldManager::read_message(LogicalPacket&& packet) {
    if (packet.id.type() == "world") {
        auto event_arg = packet.id.get_arg(0);
        if (!event_arg) return err("No event arg provided. Must be one of 'loaded', 'unloaded' or 'joined'.");
        auto& event = *event_arg;

        if (event == "loaded" || event == "joined") {
            if (_world && _world->is_authority()) return err("Authoritative world is already loaded.");

            str buffer; packet.packet >> buffer;
            auto result = glz::read_json<World>(buffer);
            if (!result) return err("Deserialisation failed.");
            
            internal_load(move(*result), false);
            return empty_success;
        }
        else if (event == "unloaded") {
            print<debug, WorldManager>("RECIEVED UNLOAD REQUEST");
            internal_unload();
            return empty_success;
        }
        else return err(fmt::format("Invalid event arg '{}'.", event));
    }
    return err("Invalid packet type.");
}