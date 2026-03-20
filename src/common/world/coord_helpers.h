#pragma once

#include <prelude/vec.h>
#include <prelude/opt.h>
#include <alias/utility.h>

namespace coords {
    ivec2 world_to_tile(const fvec2& world);
    fvec2 tile_to_world(const ivec2& tile);
    
    ivec2 tile_to_chunk(const ivec2& tile);
    tuple<ivec2, uvec2> tile_to_chunk_local_tile(const ivec2& tile);
    ivec2 chunk_to_tile(const ivec2& chunk, const opt<uvec2>& tile_local = nullopt);

    ivec2 world_to_chunk(const fvec2& world);
    tuple<ivec2, uvec2> world_to_chunk_local_tile(const fvec2& world);

    fvec2 chunk_to_world(const ivec2& chunk, const opt<uvec2>& tile_local = nullopt);
}