#include "world.h"
#include <assert.h>
#include <console.h>

#include <util/vec_convert.h>

#include <data/data_manager.h>

#include <prelude/format.h>
#include <alias/ranges.h>

#include <glaze/glaze.hpp>
#include <glaze/json.hpp>

World::World(const ::id& id, int32 seed) : _world_id(id), _world_seed(seed), _chunk_map(), IDrawable(), INetworked(calculate_network_id(id, seed)) {}
World::World(const ::id& id) : World(id, 0) {}

str World::calculate_network_id(const ::id& id, int32 seed) { return fmt::format("world!{}!{}", id, seed); }

const id& World::id() const { return _world_id; }
int32 World::seed() const { return _world_seed; }

void World::set_id(const ::id& id) { _world_id = id; set_network_id(calculate_network_id(_world_id, _world_seed)); }
void World::set_seed(int32 seed) { _world_seed = seed; set_network_id(calculate_network_id(_world_id, _world_seed)); }

void World::write_chunk_to_packet(const ivec2& chunk_coords, sf::Packet& packet) const {
    if (auto chunk = chunk_at(chunk_coords)) {
        auto result = glz::write_json(*chunk);
        if (!result) print<error, World>("Failed to serialise chunk at [{}].", fmt::join(chunk_coords, ", "));
        packet << result.value_or("null");
    }
    else packet << "null";
}

dyn_arr<LogicalPacket> World::get_outstanding_messages() const {
    dyn_arr<LogicalPacket> messages;
    return messages;
}

result<LogicalPacket, str> World::get_requested_message(const PacketId& id) const {
    if (id.type == "chunk") {
        LogicalPacket packet(id);
        for (size_t i = 0; i < id.args.size(); ++i) {
            auto numbers = id.args.at(i) | views::split(str(",")) | ranges::to<dyn_arr<str>>();
            if (numbers.size() != 2) return err(fmt::format("Arg {} did not have a length of 2.", i));
            ivec2 chunk_pos;
            try { chunk_pos = ivec2(str_to<int>(numbers[0]), str_to<int>(numbers[1])); }
            catch(std::invalid_argument e) { return err(fmt::format("Arg {} was not an ivec2.", i)); }
            write_chunk_to_packet(chunk_pos, packet.packet);
        }
        return packet;
    }
    return err("");
}

result<none_t, str> World::read_message(LogicalPacket&& packet) {
    if (packet.id.type == "chunk") {
        for (size_t i = 0; i < packet.id.args.size(); ++i) {
            auto numbers = packet.id.args.at(i) | views::split(str(",")) | ranges::to<dyn_arr<str>>();
            if (numbers.size() != 2) return err(fmt::format("Arg {} did not have a length of 2.", i));
            ivec2 chunk_pos;
            try { chunk_pos = ivec2(str_to<int>(numbers[0]), str_to<int>(numbers[1])); }
            catch(std::invalid_argument e) { return err(fmt::format("Arg {} was not an ivec2.", i)); }

            str buffer; packet.packet >> buffer;
            auto result = glz::read_json<Chunk>(buffer);
            if (result) {
                print<success, World>("Recieved chunk at [{}].", fmt::join(chunk_pos, ", "));
            }
            else return err(fmt::format("Chunk json for arg {} failed to parse ({},{}).",
                packet.id.args.at(i), std::to_underlying(result.error().ec), result.error().custom_error_message)
            );
        }
        return none;
    }
    return err("");
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