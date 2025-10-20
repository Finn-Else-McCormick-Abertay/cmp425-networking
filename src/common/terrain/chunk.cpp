#include "chunk.h"
#include <cassert>
#include <iterator>
#include <util/prelude.h>

using namespace std;

Chunk::Chunk() : _tiles{} {}

const ivec2& Chunk::get_coords() const { return _chunk_coords; }

bool Chunk::is_in_range(const uvec2& pos) { return pos.x < SIZE_TILES.x && pos.y < SIZE_TILES.y; }
bool Chunk::is_in_range(size_t index) { return index < SIZE_TILES.x * SIZE_TILES.y; }

size_t Chunk::index_from_coords(const uvec2& pos) {
    assert(is_in_range(pos));
    return pos.y * SIZE_TILES.x + pos.x; 
}

uvec2 Chunk::coords_from_index(size_t index) {
    assert(is_in_range(index));
    return uvec2( index % SIZE_TILES.x, index / SIZE_TILES.x );
}

Tile* Chunk::tile_at(const uvec2& pos) { return is_in_range(pos) ? &_tiles.at(index_from_coords(pos)) : nullptr; }
const Tile* Chunk::tile_at(const uvec2& pos) const { return is_in_range(pos) ? &_tiles.at(index_from_coords(pos)) : nullptr; }
bool Chunk::set_tile_at(const uvec2& pos, Tile tile) {
    if (!is_in_range(pos)) return false;
    _tiles[index_from_coords(pos)] = tile;
    return true;
}

Chunk::iterator Chunk::begin() { return iterator(_tiles, 0); }
Chunk::iterator Chunk::end() { return iterator(_tiles, _tiles.size()); }

Chunk::const_iterator Chunk::cbegin() const { return const_iterator(_tiles, 0); }
Chunk::const_iterator Chunk::cend() const { return const_iterator(_tiles, _tiles.size()); }


// Iterators

Chunk::iterator::iterator(tile_array_flat& array, size_t index) : _arr(&array), _i(index) {}

Chunk::iterator::return_pair Chunk::iterator::operator*() const { return make_pair(Chunk::coords_from_index(_i), &_arr->at(_i)); }

Chunk::iterator& Chunk::iterator::operator++() { _i++; return *this; }  
Chunk::iterator Chunk::iterator::operator++(int) { iterator tmp = *this; ++(*this); return tmp; }

bool operator== (const Chunk::iterator& a, const Chunk::iterator& b) { return a._arr == b._arr && a._i == b._i; }
bool operator!= (const Chunk::iterator& a, const Chunk::iterator& b) { return !(a == b); }


Chunk::const_iterator::const_iterator(const tile_array_flat& array, size_t index) : _arr(&array), _i(index) {}

Chunk::const_iterator::return_pair Chunk::const_iterator::operator*() const { return make_pair(Chunk::coords_from_index(_i), &_arr->at(_i)); }

Chunk::const_iterator& Chunk::const_iterator::operator++() { _i++; return *this; }  
Chunk::const_iterator Chunk::const_iterator::operator++(int) { const_iterator tmp = *this; ++(*this); return tmp; }

bool operator== (const Chunk::const_iterator& a, const Chunk::const_iterator& b) { return a._arr == b._arr && a._i == b._i; }
bool operator!= (const Chunk::const_iterator& a, const Chunk::const_iterator& b) { return !(a == b); }