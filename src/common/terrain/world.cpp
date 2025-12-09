#include "world.h"
#include <assert.h>
#include <console.h>

#include <util/vec_convert.h>

#include <data/data_manager.h>

#include <prelude/format.h>

World::World(id id, int32 seed)
    : _world_type_id(id), _world_seed(seed), _chunk_map(), IDrawable(),
      INetworked(fmt::format("world!{}!{}", id, seed)) {}
World::World(id id) : World(id, 0) {}

id World::type_id() const { return _world_type_id; }

int32 World::seed() const { return _world_seed; }

dyn_arr<LogicalPacket> World::write_messages() const {
    dyn_arr<LogicalPacket> messages;

    sf::Packet packet;
    messages.emplace_back("TEST", move(packet));

    return messages;
}

void World::read_message(LogicalPacket&& packet) {
    //print<info, World>("Recieved message. id: {}", packet.packet_id);
}

Chunk* World::chunk_at(const ivec2& chunk_coords) { return _chunk_map.contains(chunk_coords) ? &(_chunk_map.at(chunk_coords)) : nullptr; }
const Chunk* World::chunk_at(const ivec2& chunk_coords) const { return _chunk_map.contains(chunk_coords) ? &(_chunk_map.at(chunk_coords)) : nullptr; }

Chunk* World::set_chunk(const ivec2& chunk_coords, opt<Chunk>&& chunk, bool replace) {
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
    return set_chunk(chunk_coords, make_opt<Chunk>());
}


dyn_arr<Chunk> World::get_flattened_chunks() const {
    dyn_arr<Chunk> chunks_flat;
    for (auto& [pos, chunk] : _chunk_map) if (!chunk.empty()) chunks_flat.push_back(chunk);
    return chunks_flat;
    return {};
}
void World::set_chunks_from_flattened(const dyn_arr<Chunk>& chunks) {
    for (auto& chunk : chunks) set_chunk(chunk._chunk_coords, make_opt(move(chunk)));
}


World::iterator World::begin() { return World::iterator(_chunk_map.begin()); }
World::iterator World::end() { return World::iterator(_chunk_map.end()); }

World::const_iterator World::cbegin() const { return World::const_iterator(_chunk_map.cbegin()); }
World::const_iterator World::cend() const { return World::const_iterator(_chunk_map.cend()); }



// Iterators

World::iterator::iterator(bstmap<ivec2, Chunk>::iterator it) : _internal_it(it) {}

World::iterator::reference World::iterator::operator*() const { return _internal_it->second; }
World::iterator::pointer World::iterator::operator->() { return &_internal_it->second; }

World::iterator& World::iterator::operator++() { _internal_it++; return *this; }  
World::iterator World::iterator::operator++(int) { iterator tmp = *this; ++(*this); return tmp; }

bool operator== (const World::iterator& a, const World::iterator& b) { return a._internal_it == b._internal_it; }
bool operator!= (const World::iterator& a, const World::iterator& b) { return a._internal_it != b._internal_it; }


World::const_iterator::const_iterator(bstmap<ivec2, Chunk>::const_iterator it) : _internal_it(it) {}

World::const_iterator::reference World::const_iterator::operator*() const { return _internal_it->second; }
World::const_iterator::pointer World::const_iterator::operator->() { return &_internal_it->second; }

World::const_iterator& World::const_iterator::operator++() { _internal_it++; return *this; }  
World::const_iterator World::const_iterator::operator++(int) { const_iterator tmp = *this; ++(*this); return tmp; }

bool operator== (const World::const_iterator& a, const World::const_iterator& b) { return a._internal_it == b._internal_it; }
bool operator!= (const World::const_iterator& a, const World::const_iterator& b) { return a._internal_it != b._internal_it; }



dyn_arr<draw_layer> World::draw_layers() const { return {
    layers::tile::foreground, layers::tile::background
};}

#ifdef CLIENT
#include <util/vec_convert.h>
#include <assets/asset_manager.h>

#include <input/actions.h>

void World::draw(sf::RenderTarget& target, draw_layer layer) {
    sf::RectangleShape tile_rect(sf::fvec2(TILE_SIZE, TILE_SIZE));
    for (auto& chunk : *this) {
        if (!chunk.has(layer)) continue;
        ivec2 chunk_true_coords(chunk.get_coords() * Chunk::SIZE_TILES * TILE_SIZE);
        auto& chunk_layer = chunk.at(layer);
        for (uint i = 0; i < Chunk::TILE_COUNT; ++i) {
            auto tile = chunk_layer._tiles.at(i);

            auto tile_handle_opt = data::Manager::get_tile(tile);
            if (!tile_handle_opt) continue;
            const data::TileHandle& handle = tile_handle_opt.value();
            auto model_type = handle.model_type();
            if (model_type == data::TileHandle::ModelType::None) continue;

            auto local_pos = Chunk::coords_from_index(i);
            uint8 shape = (uint8)chunk_layer._shapes.at(i);
            
            tile_rect.setTexture(&assets::Manager::get_tile_texture(tile));
            tile_rect.setPosition(to_sfvec_of<float>(chunk_true_coords + to_fvec(local_pos) * TILE_SIZE));
            
            uvec2 texture_tile_index = uvec2(shape % tile_impl::TILESET_WIDTH, shape / tile_impl::TILESET_WIDTH);
            tile_rect.setTextureRect(sf::IntRect(
                to_sfvec_of<int>(texture_tile_index * TILE_SIZE),
                to_sfvec(ivec2(TILE_SIZE, TILE_SIZE))
            ));
            target.draw(tile_rect);
        }
    }
}

#endif