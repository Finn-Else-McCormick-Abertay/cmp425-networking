#pragma once

#include <terrain/chunk.h>
#include <optional>
#include <map>

class World {
public:
    World();

    std::optional<Chunk> chunk_at(ivec2 chunk_coords) const;
    bool set_chunk(ivec2 chunk_coords, const std::optional<Chunk>&, bool replace = true);

    const std::map<ivec2, Chunk>& chunks() const;

private:
    std::map<ivec2, Chunk> _chunk_map;
};