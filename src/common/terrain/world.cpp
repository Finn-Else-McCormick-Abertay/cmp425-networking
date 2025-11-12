#include "world.h"
#include <assert.h>
#include <util/console.h>

using namespace std;

World::World() : _chunk_map(), IDrawable() {}

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



std::vector<uint> World::draw_layers() const { return { layers::TILE_FOREGROUND, layers::TILE_BACKGROUND }; }

#ifdef CLIENT
#include <util/vec_convert.h>
#include <assets/asset_manager.h>

void World::draw(sf::RenderTarget& target, uint layer) {
    auto chunk_true_size = Chunk::SIZE_TILES * Tile::SIZE;
    
    if (layer == layers::TILE_FOREGROUND) {
        sf::RectangleShape tile_rect(sf_fvec2(Tile::SIZE, Tile::SIZE));
        for (auto& chunk : *this) {
            ivec2 chunk_true_coords(chunk.get_coords() * chunk_true_size);
            // Draw tiles
            for (auto [local_pos, tile] : chunk) {
                if (tile->type().model_type() == data::TileHandle::ModelType::Block) {
                    tile_rect.setTexture(&assets::Manager::get_tile_texture(tile->type().id()));
                    tile_rect.setPosition(to_sfvec_of<float>(chunk_true_coords + to_fvec(local_pos) * Tile::SIZE));

                    uint8 shape = (uint8)tile->shape();
                    uvec2 texture_tile_index = uvec2(shape % 6, shape / 6);
                    tile_rect.setTextureRect(sf::IntRect(
                        to_sfvec_of<int>(texture_tile_index * Tile::SIZE),
                        to_sfvec(ivec2(Tile::SIZE, Tile::SIZE))
                    ));
                    target.draw(tile_rect);
                }
            }
        }
    }
    else if (layer == layers::TILE_BACKGROUND) {
        auto chunk_debug_rect = sf::RectangleShape(sf_fvec2(chunk_true_size - 2, chunk_true_size - 2));
        chunk_debug_rect.setOutlineColor(sf::Color(255, 255, 255, 100));
        chunk_debug_rect.setOutlineThickness(1);
        chunk_debug_rect.setFillColor(sf::Color::Transparent);
        for (auto& chunk : *this) {
            ivec2 chunk_true_coords(chunk.get_coords() * chunk_true_size);
            chunk_debug_rect.setPosition(to_sfvec_of<float>(chunk_true_coords + ivec2(1, 1)));
            target.draw(chunk_debug_rect);
        }
    }
}

#endif