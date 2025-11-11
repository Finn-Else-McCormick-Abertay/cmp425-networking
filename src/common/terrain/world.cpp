#include "world.h"
#include <assert.h>

using namespace std;

World::World() : _chunk_map() {}

Chunk* World::chunk_at(const ivec2& chunk_coords) { return _chunk_map.contains(chunk_coords) ? &(_chunk_map.at(chunk_coords)) : nullptr; }
const Chunk* World::chunk_at(const ivec2& chunk_coords) const { return _chunk_map.contains(chunk_coords) ? &(_chunk_map.at(chunk_coords)) : nullptr; }

Chunk* World::set_chunk(const ivec2& chunk_coords, optional<Chunk>&& chunk, bool replace) {
    if (!replace && chunk_at(chunk_coords)) return chunk_at(chunk_coords);
    if (chunk) {
        _chunk_map[chunk_coords] = move(chunk.value());
        _chunk_map[chunk_coords]._chunk_coords = chunk_coords;
        auto this_chunk = chunk_at(chunk_coords);

        for (int i = -1; i <= 1; ++i) { for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) continue;
            this_chunk->set_chunk_neighbour(ivec2(i, j), chunk_at(chunk_coords + ivec2(i, j)));
        }}
        return this_chunk;
    }
    else if (auto existing_chunk = chunk_at(chunk_coords)) {
        for (int i = -1; i <= 1; ++i) { for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) continue;
            existing_chunk->set_chunk_neighbour(ivec2(i, j), nullptr);
        }}
        _chunk_map.erase(chunk_coords);
    }
    return nullptr;
}

Chunk* World::get_or_make_chunk_at(const ivec2& chunk_coords) {
    if (auto existing_chunk = chunk_at(chunk_coords)) return existing_chunk;
    return set_chunk(chunk_coords, make_optional<Chunk>());
}


std::vector<Chunk> World::get_flattened_chunks() const {
    std::vector<Chunk> chunks_flat;
    for (auto& [pos, chunk] : _chunk_map) if (!chunk.empty()) chunks_flat.push_back(chunk);
    return chunks_flat;
    return {};
}
void World::set_chunks_from_flattened(const std::vector<Chunk>& chunks) {
    for (auto& chunk : chunks) set_chunk(chunk._chunk_coords, make_optional(move(chunk)));
}


World::iterator World::begin() { return World::iterator(_chunk_map.begin()); }
World::iterator World::end() { return World::iterator(_chunk_map.end()); }

World::const_iterator World::cbegin() const { return World::const_iterator(_chunk_map.cbegin()); }
World::const_iterator World::cend() const { return World::const_iterator(_chunk_map.cend()); }



// Iterators

World::iterator::iterator(std::map<ivec2, Chunk>::iterator it) : _internal_it(it) {}

World::iterator::reference World::iterator::operator*() const { return _internal_it->second; }
World::iterator::pointer World::iterator::operator->() { return &_internal_it->second; }

World::iterator& World::iterator::operator++() { _internal_it++; return *this; }  
World::iterator World::iterator::operator++(int) { iterator tmp = *this; ++(*this); return tmp; }

bool operator== (const World::iterator& a, const World::iterator& b) { return a._internal_it == b._internal_it; }
bool operator!= (const World::iterator& a, const World::iterator& b) { return a._internal_it != b._internal_it; }


World::const_iterator::const_iterator(std::map<ivec2, Chunk>::const_iterator it) : _internal_it(it) {}

World::const_iterator::reference World::const_iterator::operator*() const { return _internal_it->second; }
World::const_iterator::pointer World::const_iterator::operator->() { return &_internal_it->second; }

World::const_iterator& World::const_iterator::operator++() { _internal_it++; return *this; }  
World::const_iterator World::const_iterator::operator++(int) { const_iterator tmp = *this; ++(*this); return tmp; }

bool operator== (const World::const_iterator& a, const World::const_iterator& b) { return a._internal_it == b._internal_it; }
bool operator!= (const World::const_iterator& a, const World::const_iterator& b) { return a._internal_it != b._internal_it; }