#include "interaction_system.h"

#include <console.h>
#include <util/vec_convert.h>
#include <input/actions.h>
#include <render/render_manager.h>
#include <cmath>
#include <glaze/json.hpp>
#include <world/world_manager.h>
#include <world/coord_helpers.h>
#include <network/network_manager.h>
#include <actor/actor_manager.h>

void player::InteractionSystem::tick(float dt) {
    auto level_opt = WorldManager::level("world"_id);
    if (level_opt) {
        Level& level = *level_opt;

        if (actions::place.down()) {
            auto world_pos = RenderManager::pixel_to_world(actions::place.value());
            auto [chunk_pos, local_tile_pos] = coords::world_to_chunk_local_tile(world_pos);

            Chunk& chunk = level.get_or_make_chunk_at(chunk_pos);
            chunk[tile_layer::Foreground].set_tile_at(local_tile_pos, "stone"_id);
        }
        if (actions::destroy.down()) {
            auto world_pos = RenderManager::pixel_to_world(actions::place.value());
            auto [chunk_pos, local_tile_pos] = coords::world_to_chunk_local_tile(world_pos);
            
            Chunk& chunk = level.get_or_make_chunk_at(chunk_pos);
            chunk[tile_layer::Foreground].set_tile_at(local_tile_pos, "air"_id);
        }
    }
    
    auto player_opt = NetworkManager::user_uid().and_then(ActorManager::get_player_actor);
    if (player_opt) {
        PlayerActor& player = *player_opt;

        float move_speed = 100.f;
        if (actions::run.down()) move_speed = 200.f;

        fvec2 move_vec = actions::move.value();
        move_vec.y = 0;
        move_vec *= move_speed;

        float gravity = 32.f * (float)TILE_SIZE; // Assuming 1 tile = 1 foot
        if (actions::jump.down()) gravity *= 0.5;
        fvec2 gravity_vec = fvec2(0, gravity);

        player.set_acceleration(move_vec + gravity_vec);

        if (actions::jump.just_pressed() && player.grounded()) {
            player.set_velocity(fvec2(player.velocity().x, -140.f));
        }
    }
}
