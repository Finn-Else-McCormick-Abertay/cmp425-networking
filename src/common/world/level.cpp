#include "level.h"
#include <assert.h>
#include <console.h>

#include <util/vec_convert.h>

#include <data/data_manager.h>

#include <prelude/format.h>
#include <alias/ranges.h>

#include <glaze/glaze.hpp>
#include <glaze/json.hpp>

opt_ref<Chunk> Level::chunk_at(const ivec2& chunk_coords) {
    if (_chunk_map.contains(chunk_coords)) return ref(_chunk_map.at(chunk_coords));
    return nullopt;
}
opt_cref<Chunk> Level::chunk_at(const ivec2& chunk_coords) const {
    if (_chunk_map.contains(chunk_coords)) return cref(_chunk_map.at(chunk_coords));
    return nullopt;
}

opt_ref<Chunk> Level::set_chunk(const ivec2& chunk_coords, opt<Chunk>&& chunk, bool replace) {
    if (!replace && chunk_at(chunk_coords)) return chunk_at(chunk_coords);
    if (chunk) {
        _chunk_map[chunk_coords] = move(chunk.value());
        _chunk_map[chunk_coords]._pos = chunk_coords;
        Chunk& this_chunk = chunk_at(chunk_coords).value();

        for (int i = -1; i <= 1; ++i) { for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) continue;
            this_chunk.set_chunk_neighbour(ivec2(i, j), chunk_at(chunk_coords + ivec2(i, j)));
        }}
        return this_chunk;
    }
    else if (auto existing_chunk = chunk_at(chunk_coords)) {
        for (int i = -1; i <= 1; ++i) { for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) continue;
            existing_chunk.value().get().set_chunk_neighbour(ivec2(i, j), nullopt);
        }}
        _chunk_map.erase(chunk_coords);
    }
    return nullopt;
}

Chunk& Level::get_or_make_chunk_at(const ivec2& chunk_coords) {
    if (auto existing_chunk = chunk_at(chunk_coords)) return existing_chunk.value();
    return set_chunk(chunk_coords, make_opt<Chunk>()).value();
}


#ifdef CLIENT
#include <util/vec_convert.h>
#include <assets/asset_manager.h>

#include <input/actions.h>

dyn_arr<draw_layer> Level::draw_layers() const { return {
    layers::tile::foreground, layers::tile::background
};}

void Level::draw(sf::RenderTarget& target, draw_layer draw_layer) {
    sf::RectangleShape tile_rect(sf::fvec2(TILE_SIZE, TILE_SIZE));
    for (auto& chunk : chunks()) {
        tile_layer tile_layer;
        switch (draw_layer) {
            case layers::tile::foreground: tile_layer = tile_layer::Foreground; break;
            case layers::tile::background: tile_layer = tile_layer::Background; break;
        }
        if (!chunk.has(tile_layer)) continue;
        ivec2 chunk_true_coords(chunk.pos() * Chunk::SIZE_TILES * TILE_SIZE);
        auto& chunk_layer = chunk.at(tile_layer);
        for (uint i = 0; i < Chunk::SIZE_TILES * Chunk::SIZE_TILES; ++i) {
            auto tile = chunk_layer._tiles.at(i);

            auto tile_handle_opt = DataManager::get_tile(id(tile));
            if (!tile_handle_opt) continue;
            const data::TileHandle& handle = tile_handle_opt.value();
            auto model_type = handle.model_type();
            if (model_type == data::TileHandle::ModelType::None) continue;

            auto local_pos = Chunk::to_pos(i);
            uint8 shape = (uint8)chunk_layer._shapes.at(i);
            
            tile_rect.setTexture(&AssetManager::get_texture(asset_id(tile, AssetType::texture_tileset)));
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