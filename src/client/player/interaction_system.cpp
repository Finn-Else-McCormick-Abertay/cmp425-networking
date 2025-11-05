#include "interaction_system.h"

#include <util/console.h>
#include <util/vec_convert.h>
#include <input/actions.h>
#include <camera/camera_manager.h>

#include <cmath>

using namespace std;

player::InteractionSystem::InteractionSystem(World* world) : _world(world) {}

void player::InteractionSystem::tick(float dt) {
    if (!_world) return;
    if (Camera* active_camera = CameraManager::get_active_camera()) {
        if (actions::place.down()) {
            auto world_tile_pos = CameraManager::pixel_to_world(actions::click.value()) / Tile::SIZE;
            auto chunk_pos = ivec2(floorf(world_tile_pos.x / Chunk::SIZE_TILES), floorf(world_tile_pos.y / Chunk::SIZE_TILES));
            auto local_tile_pos = to_uvec(world_tile_pos - (chunk_pos * (float)Chunk::SIZE_TILES));

            Chunk* chunk = _world->get_or_make_chunk_at(chunk_pos);
            chunk->set_tile_at(local_tile_pos, Tile("default::stone"_id));
        }
        if (actions::destroy.down()) {
            auto world_tile_pos = CameraManager::pixel_to_world(actions::destroy.value()) / Tile::SIZE;
            auto chunk_pos = ivec2(floorf(world_tile_pos.x / Chunk::SIZE_TILES), floorf(world_tile_pos.y / Chunk::SIZE_TILES));
            auto local_tile_pos = to_uvec(world_tile_pos - (chunk_pos * (float)Chunk::SIZE_TILES));
            
            Chunk* chunk = _world->get_or_make_chunk_at(chunk_pos);
            chunk->set_tile_at(local_tile_pos, Tile("default::air"_id));
        }
    }
}