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

using namespace vmath_hpp;

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

void ActorManager::fixed_tick() {
    auto level_opt = WorldManager::level("world"_id);

    auto delta = SystemManager::FIXED_TIMESTEP / 1.0s;

    for (auto actor : inst()._known_actors) {
        actor->set_velocity(actor->velocity() + actor->acceleration() * delta);
        actor->set_pos(actor->pos() + actor->velocity() * delta);

        actor->set_grounded(false);

        if (level_opt) {
            auto& level = level_opt.value().get();

            auto actor_chunk_pos = coords::world_to_chunk(actor->pos());
            
            for (int i = -1; i <= 1; ++i) for (int j = -1; j <= 1; ++j) {
                auto chunk_pos = actor_chunk_pos + ivec2(i, j);
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

                        if (overlap(actor->global_rect(), tile_global_rect)) {
                            fvec2 overlap_amount = overlap_by(actor->global_rect(), tile_global_rect);
                            fvec2 position_correction = fvec2();
                            fvec2 velocity_correction = fvec2();
                            if (overlap_amount.x < overlap_amount.y) {
                                // Prevent getting stuck while sliding on floors
                                if (overlap_amount.y < 2) continue;

                                bool is_to_right = actor->pos().x > tile_global_origin.x + (TILE_SIZE / 2);

                                position_correction.x = overlap_amount.x * -1;
                                if (is_to_right) position_correction.x *= -1;

                                if ((is_to_right && actor->velocity().x < 0) || (!is_to_right && actor->velocity().x > 0)) {
                                    velocity_correction.x = -actor->velocity().x;
                                }
                            }
                            else {
                                // Prevent getting stuck while sliding on walls
                                if (overlap_amount.x < 2) continue;

                                bool is_above = actor->pos().y < tile_global_origin.y + (TILE_SIZE / 2);

                                position_correction.y = overlap_amount.y;
                                if (is_above) position_correction.y *= -1;

                                if ((is_above && actor->velocity().y > 0) || (!is_above && actor->velocity().y < 0)) {
                                    velocity_correction.y = -actor->velocity().y;
                                    // Friction
                                    float max_friction = actor->velocity().y * 0.2f;
                                    float true_friction = min(abs(actor->velocity().x), max_friction);
                                    velocity_correction.x = -sign(actor->velocity().x) * true_friction;
                                }

                                if (is_above) actor->set_grounded(true);
                            }

                            actor->set_pos(actor->pos() + position_correction);
                            actor->set_velocity(actor->velocity() + velocity_correction);
                            //print<info, ActorManager>("Collision : {} {}", position_correction, velocity_correction);
                        }
                    }
                    // Assert that all collision types are handled (in case I add more later on)
                    else print<error, ActorManager>("Unhandled collision type {}.", std::to_underlying(collision_type));
                }
            }
        }
    }

    #ifdef CLIENT
    // Handle player camera
    for (auto& [ident, player_actor] : _players) {
        if (player_actor.is_authority()) {
            auto cam_opt = RenderManager::get_camera("player");
            if (cam_opt) {
                auto& cam = cam_opt.value().get();
                cam.set_position(player_actor.pos());
            }
            break;
        }
    }
    #endif
}

// Something's wrong here. I don't think try emplace works the way I think it does. Hacking around it elsewhere
PlayerActor& ActorManager::register_player(const str& ident, bool broadcast, bool fail_quiet) {
    auto [it, is_new] = inst()._players.try_emplace(ident, PlayerActor(ident));
    if (is_new && broadcast) NetworkManager::broadcast(LogicalPacket(inst().network_id(), packet_id("player", { "connected", ident })));

    if (is_new) print<info, ActorManager>("Registered player '{}'.", ident);
    else if (!fail_quiet) print<warning, ActorManager>("register_player called for previously registered player '{}'.", ident);
    return it->second;
}

void ActorManager::unregister_player(const str& ident, bool broadcast, bool fail_quiet) {
    if (!inst()._players.erase(ident) && !fail_quiet) return print<warning, ActorManager>("unregister_player called for nonexistent player '{}'.", ident);
    
    if (broadcast) NetworkManager::broadcast(LogicalPacket(inst().network_id(), packet_id("player", { "disconnected", ident })));
    print<info, ActorManager>("Unregistered player '{}'.", ident);
}

void ActorManager::update_player_authority_states() {
    for (auto& [_, player] : inst()._players) player.update_authority_state();
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
        
        if (event == "connected" || event == "existing") return LogicalPacket(network_id(), id);
        else if (event == "disconnected") return LogicalPacket(network_id(), id);
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