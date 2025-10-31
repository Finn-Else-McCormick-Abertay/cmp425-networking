#include "interaction_system.h"

#include <util/console.h>
#include <util/vec_convert.h>
#include <input/actions.h>
#include <camera/camera_manager.h>

using namespace std;

player::InteractionSystem::InteractionSystem(World* world) : _world(world) {}

void player::InteractionSystem::tick(float dt) {
    if (!_world) return;
    if (Camera* active_camera = CameraManager::get_active_camera()) {
        if (actions::place.down()) {
            auto world_pos = CameraManager::pixel_to_world(actions::click.value());
            auto world_tile_pos = world_pos / Tile::SIZE;

            auto chunk_pos = to_ivec(world_tile_pos / Chunk::SIZE_TILES);
            Chunk* chunk = _world->chunk_at(chunk_pos);
            if (!chunk) chunk = _world->set_chunk(chunk_pos, make_optional<Chunk>());

            auto local_tile_pos = to_ivec(world_tile_pos - to_fvec(chunk_pos));
            chunk->set_tile_at(local_tile_pos, Tile(Tile::Stone));
            
            console::debug("Attempt place at {} / {} / {}.{}", actions::click.value(), world_tile_pos, chunk_pos, local_tile_pos);
        }
    }
}