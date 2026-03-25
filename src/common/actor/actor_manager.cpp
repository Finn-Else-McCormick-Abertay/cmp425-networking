#include "actor_manager.h"

#include <network/network_manager.h>
#include <world/world_manager.h>
#include <world/coord_helpers.h>
#include <data/data_manager.h>
#include <game_loop.h>
#include <console.h>
#include <vmath.hpp/vmath_fun.hpp>
#ifdef CLIENT
#include <render/render_manager.h>
#endif
#include <system/system_manager.h>
#include <fmt/core.h>

using namespace vmath_hpp;

DEFINE_SINGLETON(ActorManager);

ActorManager::ActorManager() : INetworked("singleton#actor_manager"_netid) {}

void ActorManager::Registry::__register(IActor& actor)   { inst()._known_actors.insert(&actor); }
void ActorManager::Registry::__unregister(IActor& actor) { inst()._known_actors.erase(&actor); }

void ActorManager::Registry::__register(INetworkedActor& actor)   { inst()._known_networked_actors.insert(&actor); }
void ActorManager::Registry::__unregister(INetworkedActor& actor) { inst()._known_networked_actors.erase(&actor); }

void ActorManager::init() {
    // This is necessary cause of the weird pattern I've locked myself into where static initialisation order is something I have to consider
    inst();
}

actor::InterpolationMode ActorManager::interpolation_mode(bool validate) {
    if (validate && inst()._interpolation_mode == actor::InterpolationMode::DEFAULT) {
        #ifdef CLIENT
        return ActorManager::CLIENT_DEFAULT_INTERPOLATION;
        #elifdef SERVER
        return ActorManager::SERVER_DEFAULT_INTERPOLATION;
        #endif
    }
    return inst()._interpolation_mode;
}
void ActorManager::set_interpolation_mode(actor::InterpolationMode new_mode) {
    inst()._interpolation_mode = new_mode;
    print<info, ActorManager>("Set interpolation mode to {}{}", new_mode, new_mode == actor::InterpolationMode::DEFAULT ? fmt::format(" ({})", interpolation_mode()) : "");
}

void ActorManager::perform_physics_step(IActor& actor) {
    if (actor.physics_mode() != actor::PhysicsMode::DYNAMIC) return;

    auto delta = SystemManager::FIXED_TIMESTEP / 1.0s;
    actor.set_velocity(actor.velocity() + actor.acceleration() * delta);
    actor.set_position(actor.position() + actor.velocity() * delta);

    handle_collisions(actor);
}

void ActorManager::handle_collisions(IActor& actor, bool apply_friction) {
    actor.set_grounded(false);

    auto level_opt = WorldManager::level("world"_id);
    if (!level_opt) return;
    auto& level = level_opt.value().get();
    
    // These positions are inserted into a set, so if more than one of the corners is in the same chunk that chunk is only checked once.
    set<ivec2> chunks_to_check;
    frect2 global_rect = actor.global_rect();
    chunks_to_check.insert(coords::world_to_chunk(global_rect.origin));
    chunks_to_check.insert(coords::world_to_chunk(global_rect.origin + fvec2(global_rect.size.x, 0)));
    chunks_to_check.insert(coords::world_to_chunk(global_rect.origin + fvec2(0, global_rect.size.y)));
    chunks_to_check.insert(coords::world_to_chunk(global_rect.origin + global_rect.size));

    for (auto& chunk_pos : chunks_to_check) {
        auto chunk_opt = level.chunk_at(chunk_pos);
        if (!chunk_opt) continue;
        auto& chunk = chunk_opt.value().get();

        if (!chunk.has(tile_layer::Foreground)) continue;
        auto& foreground = chunk.at(tile_layer::Foreground);
        for (uint x = 0; x < Chunk::SIZE_TILES; ++x) for (uint y = 0; y < Chunk::SIZE_TILES; ++y) {
            uvec2 local_tile_pos = uvec2(x,y);
            auto tile_opt = DataManager::get_tile(foreground.tile_at(local_tile_pos));
            if (!tile_opt) continue;
            auto& tile_handle = tile_opt.value().get();

            auto collision_type = tile_handle.collision_type();
            // If tile has no collision, skip
            if (collision_type == data::TileHandle::CollisionType::None) continue;

            auto tile_global_origin = coords::chunk_to_world(chunk_pos, local_tile_pos);

            // If tile is a solid block
            if (collision_type == data::TileHandle::CollisionType::Block) {
                frect2 tile_global_rect = frect2(tile_global_origin, fvec2(TILE_SIZE, TILE_SIZE));

                if (overlap(global_rect, tile_global_rect)) {
                    fvec2 overlap_amount = overlap_by(global_rect, tile_global_rect);
                    fvec2 position_correction = fvec2();
                    fvec2 velocity_correction = fvec2();
                    if (overlap_amount.x < overlap_amount.y) {
                        // Prevent getting stuck while sliding on floors
                        if (overlap_amount.y < 2) continue;

                        bool is_to_right = actor.position().x > tile_global_origin.x + (TILE_SIZE / 2);

                        position_correction.x = overlap_amount.x * -1;
                        if (is_to_right) position_correction.x *= -1;

                        if ((is_to_right && actor.velocity().x < 0) || (!is_to_right && actor.velocity().x > 0)) {
                            velocity_correction.x = -actor.velocity().x;
                        }
                    }
                    else {
                        // Prevent getting stuck while sliding on walls
                        if (overlap_amount.x < 2) continue;

                        bool is_above = actor.position().y < tile_global_origin.y + (TILE_SIZE / 2);

                        position_correction.y = overlap_amount.y;
                        if (is_above) position_correction.y *= -1;

                        if ((is_above && actor.velocity().y > 0) || (!is_above && actor.velocity().y < 0)) {
                            velocity_correction.y = -actor.velocity().y;
                            if (apply_friction) {
                                float friction_coefficient = abs(actor.velocity().x) > 1.f ? 0.15f : 0.5f;
                                float max_friction_magnitude = actor.velocity().y * friction_coefficient;
                                float true_friction_magnitude = min(abs(actor.velocity().x), max_friction_magnitude);
                                velocity_correction.x = -sign(actor.velocity().x) * true_friction_magnitude;
                            }
                        }

                        if (is_above) actor.set_grounded(true);
                    }

                    actor.set_position(actor.position() + position_correction);
                    actor.set_velocity(actor.velocity() + velocity_correction);
                    
                    // Update global rect based on changed position
                    global_rect = actor.global_rect();
                }
            }
            // Assert that all collision types are handled (in case I add more later on)
            else print<error, ActorManager>("Unhandled collision type {}.", std::to_underlying(collision_type));
        }
    }

}

void ActorManager::fixed_tick() {
    // Progress physics
    for (auto actor : inst()._known_actors) perform_physics_step(*actor);

    #ifdef CLIENT
    // Handle player camera
    for (auto& [ident, player_actor] : _players) {
        if (player_actor.is_authority()) {
            auto cam_opt = RenderManager::get_camera("player");
            if (cam_opt) {
                auto& cam = cam_opt.value().get();
                cam.set_position(player_actor.position());
            }
            break;
        }
    }
    #endif
}

// Something's wrong here. I don't think try emplace works the way I think it does. Hacking around it elsewhere
PlayerActor& ActorManager::register_player(const str& ident, bool broadcast, bool fail_quiet) {
    auto [it, is_new] = inst()._players.try_emplace(ident, PlayerActor(ident));
    update_player_authority_states();
    if (is_new && broadcast) NetworkManager::broadcast(LogicalPacket(inst().netid(), packet_id("player", { "connected", ident })));

    if (is_new) print<info, ActorManager>("Registered player '{}'.", ident);
    else if (!fail_quiet) print<warning, ActorManager>("register_player called for previously registered player '{}'.", ident);
    return it->second;
}

void ActorManager::unregister_player(const str& ident, bool broadcast, bool fail_quiet) {
    if (!inst()._players.erase(ident) && !fail_quiet) return print<warning, ActorManager>("unregister_player called for nonexistent player '{}'.", ident);
    
    if (broadcast) NetworkManager::broadcast(LogicalPacket(inst().netid(), packet_id("player", { "disconnected", ident })));
    print<info, ActorManager>("Unregistered player '{}'.", ident);
}

void ActorManager::update_player_authority_states() {
    auto user_uid = NetworkManager::user_uid();
    for (auto& [ident, player] : inst()._players) {
        #ifdef SERVER
        player.set_network_mode(actor::NetworkMode::RELAY);
        #elifdef CLIENT
        player.set_network_mode(user_uid && (*user_uid == ident) ? actor::NetworkMode::AUTHORITY : actor::NetworkMode::LISTENER);
        #endif
    }
}

opt_ref<PlayerActor> ActorManager::get_player_actor(const str& ident) {
    if (!inst()._players.contains(ident)) return nullopt;
    return ref(inst()._players.at(ident));
}

result<LogicalPacket, str> ActorManager::get_requested_message(const packet_id& id) const {
    if (id.type() == "player") {
        auto event_arg = id.get_arg(0); auto ident_arg = id.get_arg(1);
        if (!event_arg) return err("Missing event arg."); else if (!ident_arg) return err("Missing player ident arg.");
        auto& event = *event_arg; auto& ident = *ident_arg;
        
        if (event == "connected" || event == "existing") return LogicalPacket(netid(), id);
        else if (event == "disconnected") return LogicalPacket(netid(), id);
        else return err("Invalid event arg. Must be one of 'connected', 'disconnected' or 'existing'.");
    }
    return err("Unknown packet type.");
}

result<success_t, str> ActorManager::read_message(LogicalPacket&& packet) {
    print<debug, ActorManager>("RECIEVED {} ({})", packet.id, NetworkManager::user_uid());
    if (packet.id.type() == "player") {
        auto event_arg = packet.id.get_arg(0); auto ident_arg = packet.id.get_arg(1);
        if (!event_arg) return err("Missing event arg."); else if (!ident_arg) return err("Missing player ident arg.");
        auto& event = *event_arg; auto& ident = *ident_arg;
        
        if (event == "connected" || event == "existing") {
            if (!inst()._players.contains(ident)) register_player(ident, false, event == "existing");
            return empty_success;
        }
        else if (event == "disconnected") {
            print<debug, ActorManager>("PLAYER UNLOAD REQUEST : {}", ident);
            unregister_player(ident, false);
            return empty_success;
        }
        else return err("Invalid event arg. Must be one of 'connected', 'disconnected' or 'existing'.");
    }
    return err("Unknown packet type.");
}

dyn_arr<str> ActorManager::debug_message() {
    dyn_arr<str> messages {
        fmt::format("Interpolation: {}", interpolation_mode()),
    };
    for (auto& actor : inst()._known_networked_actors) {
        str actor_info = fmt::format("{:<15} {:>15}", actor->netid(), fmt::format("({})", actor->network_mode()));
        str motion_info = fmt::format("Pos {:n: > 8.0f} | Vel {:n: > 8.0f} | Accel {:n: > 8.0f}", actor->position(), actor->velocity(), actor->acceleration());
        messages.push_back(fmt::format("{} : {}", actor_info, motion_info));
    }
    return messages;
}