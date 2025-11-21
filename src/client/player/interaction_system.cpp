#include "interaction_system.h"

#include <console.h>
#include <util/vec_convert.h>
#include <input/actions.h>
#include <render/render_manager.h>

#include <cmath>

#include <glaze/json.hpp>


player::InteractionSystem::InteractionSystem(World* world) : _world(world) {}

void player::InteractionSystem::tick(float dt) {
    if (!_world) return;
    if (actions::place.down()) {
        auto world_tile_pos = RenderManager::pixel_to_world(actions::place.value()) / TILE_SIZE;
        auto chunk_pos = ivec2(floorf(world_tile_pos.x / Chunk::SIZE_TILES), floorf(world_tile_pos.y / Chunk::SIZE_TILES));
        auto local_tile_pos = to_uvec(world_tile_pos - (chunk_pos * (float)Chunk::SIZE_TILES));

        Chunk& chunk = *_world->get_or_make_chunk_at(chunk_pos);
        chunk[layers::tile::foreground].set_tile_at(local_tile_pos, "default::stone"_id);
    }
    if (actions::destroy.down()) {
        auto world_tile_pos = RenderManager::pixel_to_world(actions::destroy.value()) / TILE_SIZE;
        auto chunk_pos = ivec2(floorf(world_tile_pos.x / Chunk::SIZE_TILES), floorf(world_tile_pos.y / Chunk::SIZE_TILES));
        auto local_tile_pos = to_uvec(world_tile_pos - (chunk_pos * (float)Chunk::SIZE_TILES));
        
        Chunk& chunk = *_world->get_or_make_chunk_at(chunk_pos);
        chunk[layers::tile::foreground].set_tile_at(local_tile_pos, "default::air"_id);
    }
}