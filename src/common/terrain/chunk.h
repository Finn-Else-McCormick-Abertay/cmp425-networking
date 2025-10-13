#pragma once

#include <quadtree.h>
#include <util/vec.h>

class Chunk {
public:
    static constexpr ivec2 SIZE_TILES = { 16, 16 };

    Chunk();

    const ivec2& get_coords() const;
private:
    // Set by the containing world
    ivec2 _chunk_coords;

    friend class World;
};
