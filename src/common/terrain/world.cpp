#include "world.h"
#include <assert.h>

using namespace std;

World::World() : _chunk_map() {}

optional<Chunk> World::chunk_at(ivec2 chunk_coords) const {
    auto it = _chunk_map.find(chunk_coords);
    return it == _chunk_map.cend() ? nullopt : make_optional(it->second);
}

bool World::set_chunk(ivec2 chunk_coords, const optional<Chunk>& chunk, bool replace) {
    auto existing_chunk = chunk_at(chunk_coords); 
    if (existing_chunk && !replace) return false;
    if (chunk) {
        _chunk_map[chunk_coords] = chunk.value();
        _chunk_map[chunk_coords]._chunk_coords = chunk_coords;
    }
    else if (existing_chunk) _chunk_map.erase(chunk_coords);
    return true;
}

const std::map<ivec2, Chunk>& World::chunks() const { return _chunk_map; }