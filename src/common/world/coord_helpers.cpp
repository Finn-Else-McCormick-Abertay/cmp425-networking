#include "coord_helpers.h"

#include <world/chunk.h>

ivec2 coords::world_to_tile(const fvec2& world) {
    return ivec2(floorf(world.x / (float)TILE_SIZE), floorf(world.y / (float)TILE_SIZE));
}
fvec2 coords::tile_to_world(const ivec2& tile) {
    return tile * (float)TILE_SIZE;
}

ivec2 coords::tile_to_chunk(const ivec2& tile) {
    return ivec2(floorf(tile.x / (float)Chunk::SIZE_TILES), floorf(tile.y / (float)Chunk::SIZE_TILES));
}
tuple<ivec2, uvec2> coords::tile_to_chunk_local_tile(const ivec2& tile) {
    ivec2 chunk = tile_to_chunk(tile);
    uvec2 local_tile = to_uvec(tile - (chunk * (int)Chunk::SIZE_TILES));
    return make_tuple(move(chunk), move(local_tile));
}
ivec2 coords::chunk_to_tile(const ivec2& chunk, const opt<uvec2>& tile_local) {
    ivec2 tile = chunk * (int)Chunk::SIZE_TILES;
    if (tile_local) tile += to_ivec(tile_local.value());
    return tile;
}

ivec2 coords::world_to_chunk(const fvec2& world) {
    return tile_to_chunk(world_to_tile(world));
}
tuple<ivec2, uvec2> coords::world_to_chunk_local_tile(const fvec2& world) {
    return tile_to_chunk_local_tile(world_to_tile(world));
}

fvec2 coords::chunk_to_world(const ivec2& chunk, const opt<uvec2>& tile_local) {
    return tile_to_world(chunk_to_tile(chunk, tile_local));
}