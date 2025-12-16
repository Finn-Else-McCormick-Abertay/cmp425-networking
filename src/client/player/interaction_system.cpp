#include "interaction_system.h"

#include <console.h>
#include <util/vec_convert.h>
#include <input/actions.h>
#include <render/render_manager.h>

#include <cmath>

#include <glaze/json.hpp>

#include <world/world_manager.h>

void player::InteractionSystem::tick(float dt) {
    auto level_opt = WorldManager::level("world"_id);
    if (!level_opt) return;
    Level& level = *level_opt;

    if (actions::place.down()) {
        auto world_tile_pos = RenderManager::pixel_to_world(actions::place.value()) / TILE_SIZE;
        auto chunk_pos = ivec2(floorf(world_tile_pos.x / Chunk::SIZE_TILES), floorf(world_tile_pos.y / Chunk::SIZE_TILES));
        auto local_tile_pos = to_uvec(world_tile_pos - (chunk_pos * (float)Chunk::SIZE_TILES));

        Chunk& chunk = level.get_or_make_chunk_at(chunk_pos);
        chunk[tile_layer::Foreground].set_tile_at(local_tile_pos, "stone"_id);
    }
    if (actions::destroy.down()) {
        auto world_tile_pos = RenderManager::pixel_to_world(actions::destroy.value()) / TILE_SIZE;
        auto chunk_pos = ivec2(floorf(world_tile_pos.x / Chunk::SIZE_TILES), floorf(world_tile_pos.y / Chunk::SIZE_TILES));
        auto local_tile_pos = to_uvec(world_tile_pos - (chunk_pos * (float)Chunk::SIZE_TILES));
        
        Chunk& chunk = level.get_or_make_chunk_at(chunk_pos);
        chunk[tile_layer::Foreground].set_tile_at(local_tile_pos, "air"_id);
    }
}