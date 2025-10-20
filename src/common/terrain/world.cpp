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