#include "chunk.h"
#include <cassert>
#include <iterator>

#include <prelude.h>
#include <prelude/containers.h>
#include <alias/bitset.h>

#include <data/definitions/tile.h>
#include <data/data_manager.h>

Chunk::Chunk() {}
Chunk::Chunk(const Chunk& rhs) : _pos(rhs._pos), _neighbours(rhs._neighbours) { replace_layers(rhs._layers); }
Chunk::Chunk(Chunk&& rhs) : _pos(move(rhs._pos)), _neighbours(move(rhs._neighbours)), _layers(move(rhs._layers)) { post_update_layers(); }

Chunk& Chunk::operator=(const Chunk& rhs) { _pos = rhs._pos; _neighbours = rhs._neighbours; replace_layers(rhs._layers); return *this; }
Chunk& Chunk::operator=(Chunk&& rhs) { _pos = move(rhs._pos); _neighbours = move(rhs._neighbours); _layers = move(rhs._layers); post_update_layers(); return *this; }

void Chunk::replace_layers(const bstmap<tile_layer, Layer>& layers) {
    _layers.clear(); _layers.insert_range(layers); post_update_layers();
}

void Chunk::post_update_layers() { for (auto& [i, layer] : _layers) { layer._owner = this; layer._layer = i; } }

const ivec2& Chunk::pos() const { return _pos; }

bool Chunk::is_in_range(const uvec2& pos) { return pos.x < SIZE_TILES && pos.y < SIZE_TILES; }
bool Chunk::is_in_range(size_t index) { return index < SIZE_TILES * SIZE_TILES; }

size_t Chunk::to_index(const uvec2& pos) { assert(is_in_range(pos)); return pos.y * SIZE_TILES + pos.x; }
uvec2 Chunk::to_pos(size_t index) { assert(is_in_range(index)); return uvec2( index % SIZE_TILES, index / SIZE_TILES ); }

bool Chunk::has(tile_layer layer) const { return _layers.contains(layer); }
bool Chunk::empty() const { return ranges::any_of(views::values(_layers), [](auto& x) { return x.empty(); }); }

// Layer
Chunk::Layer::Layer(Chunk* owner, tile_layer layer) : _owner(owner), _layer(layer) {
    _tiles.fill("default::air"_id); _shapes.fill(TileShape::____);
}

Chunk::Layer& Chunk::operator[](tile_layer layer) {
    if (!has(layer)) _layers[layer] = Layer(this, layer);
    return _layers.at(layer);
}
const Chunk::Layer& Chunk::operator[](tile_layer layer) const { return at(layer); }

Chunk::Layer& Chunk::at(tile_layer layer) { assert(has(layer)); return _layers.at(layer); }
const Chunk::Layer& Chunk::at(tile_layer layer) const { assert(has(layer)); return _layers.at(layer); }

bool Chunk::Layer::empty() const {
    for (auto& tile : _tiles) { if (tile != nullid && tile != "default::air"_id) return false; }
    return true;
}

const id& Chunk::Layer::tile_at(const uvec2& pos) const { return is_in_range(pos) ? _tiles.at(to_index(pos)) : nullid; }
TileShape Chunk::Layer::shape_at(const uvec2& pos) const { return _shapes.at(to_index(pos)); }
bool Chunk::Layer::set_tile_at(const uvec2& pos, const id& tile) {
    if (!is_in_range(pos)) return false;
    size_t index = to_index(pos);
    if (_tiles.at(index) == tile) return false;
    _tiles[index] = tile;
    _owner->_updated_since_sync = true;
    //print<info, Chunk>("Chunk{}::{} set tile at {} to {}", _owner->_pos, _layer, pos, tile);

    // Update shapes
    update_shape(pos);
    for (int i = -1; i <= 1; ++i) { for (int j = -1; j <= 1; ++j) {
        if (i == 0 && j == 0) continue;
        
        ivec2 nbr_dir = {i, j};
        ivec2 nbr_pos = nbr_dir + pos;
        ivec2 bounds_signs = {(nbr_pos.x >= (int)SIZE_TILES) - (nbr_pos.x < 0), (nbr_pos.y >= (int)SIZE_TILES) - (nbr_pos.y < 0)};
        
        if (bounds_signs.x == 0 && bounds_signs.y == 0) update_shape(nbr_pos);
        else if (_owner) {
            auto neighbour_chunk = _owner->get_chunk_neighbour(bounds_signs);
            if (neighbour_chunk && neighbour_chunk.value().get().has(_layer)) {
                ivec2 other_chunk_nbr_pos = nbr_pos + bounds_signs * -(int)SIZE_TILES;
                neighbour_chunk.value().get().at(_layer).update_shape(other_chunk_nbr_pos);
            }
        }
    }}
    return true;
}

opt_ref<Chunk> Chunk::get_chunk_neighbour(const ivec2& dir) {
    assert(abs(dir.x) <= 1 && abs(dir.y) <= 1 && !(dir.x == 0 && dir.y == 0));
    Chunk* ptr = _neighbours.contains(dir) ? _neighbours.at(dir) : nullptr;
    if (ptr) return ref(*ptr);
    return nullopt;
}

void Chunk::set_chunk_neighbour(const ivec2& dir, opt_ref<Chunk> neighbour, bool recursive) {
    assert(abs(dir.x) <= 1 && abs(dir.y) <= 1 && !(dir.x == 0 && dir.y == 0));
    _neighbours[dir] = neighbour.has_value() ? &neighbour.value().get() : nullptr;
    if (neighbour && recursive) neighbour.value().get().set_chunk_neighbour(-dir, ref(*this), false);
    if (!recursive) update_shapes_along_neighbour_border(dir);
}

void Chunk::update_all_layer_shapes_at(const uvec2& pos) {
    for (auto& [_, layer] : _layers) layer.update_shape(pos);
}

void Chunk::update_shapes_along_neighbour_border(const ivec2& dir, bool neighbour_only) {
    assert(abs(dir.x) <= 1 && abs(dir.y) <= 1 && !(dir.x == 0 && dir.y == 0));

    auto neighbour = get_chunk_neighbour(dir);
    if (!neighbour && neighbour_only) return;

    // Corner
    if (dir.x != 0 && dir.y != 0) {
        uvec2 corner, opposing_corner;
        if (dir.x < 0) { corner.x = 0; opposing_corner.x = SIZE_TILES - 1; }
        else if (dir.x > 0) { corner.x = SIZE_TILES - 1; opposing_corner.x = 0; }

        if (dir.y < 0) { corner.y = 0; opposing_corner.y = SIZE_TILES - 1; }
        else if (dir.y > 0) { corner.y = SIZE_TILES - 1; opposing_corner.y = 0; }

        if (!neighbour_only) update_all_layer_shapes_at(corner);
        if (neighbour) neighbour.value().get().update_all_layer_shapes_at(opposing_corner);
    }
    // Vertical
    else if (dir.y == 0) {
        uint column, opposing_column;
        if (dir.x < 0) { column = 0; opposing_column = SIZE_TILES - 1; }
        else if (dir.x > 0) { column = SIZE_TILES - 1; opposing_column = 0; }

        for (uint i = 0; i < SIZE_TILES; ++i) {
            if (!neighbour_only) update_all_layer_shapes_at(ivec2(column, i));
            if (neighbour) neighbour.value().get().update_all_layer_shapes_at(ivec2(opposing_column, i));
        }
    }
    // Horizontal
    else if (dir.x == 0) {
        uint row, opposing_row;
        if (dir.y < 0) { row = 0; opposing_row = SIZE_TILES - 1; }
        else if (dir.y > 0) { row = SIZE_TILES - 1; opposing_row = 0; }
        
        for (uint i = 0; i < SIZE_TILES; ++i) {
            if (!neighbour_only) update_all_layer_shapes_at(ivec2(i, row));
            if (neighbour) neighbour.value().get().update_all_layer_shapes_at(ivec2(i, opposing_row));
        }
    }
}

bitset<8> Chunk::Layer::find_connections(const uvec2& pos) const {
    assert(is_in_range(pos));
    auto tile_id = tile_at(pos);

    // Placeholder for more complex logic down the line
    auto should_connect = [](const id& lhs, const id& rhs) -> bool { return lhs == rhs; };

    // Left, Right, Top, Bottom, LftTop, RgtTop, LftBot, RgtBot
    bitset<8> connects_in_dir;
    static const bstmap<ivec2, uint> dir_to_index {
        {ivec2{-1,  0}, 7}, {ivec2{ 1,  0}, 6}, {ivec2{ 0, -1}, 5}, {ivec2{ 0,  1}, 4},
        {ivec2{-1, -1}, 3}, {ivec2{ 1, -1}, 2}, {ivec2{-1,  1}, 1}, {ivec2{ 1,  1}, 0}
    };

    for (int i = -1; i <= 1; ++i) { for (int j = -1; j <= 1; ++j) {
        if (i == 0 && j == 0) continue;
        bool connects = false;

        ivec2 nbr_dir = {i, j};
        ivec2 nbr_pos = nbr_dir + pos;
        ivec2 bounds_signs = {(nbr_pos.x >= (int)SIZE_TILES) - (nbr_pos.x < 0), (nbr_pos.y >= (int)SIZE_TILES) - (nbr_pos.y < 0)};

        if (bounds_signs.x == 0 && bounds_signs.y == 0) connects = should_connect(tile_id, tile_at(nbr_pos));
        else if (_owner) {
            auto neighbour_chunk = _owner->get_chunk_neighbour(bounds_signs);
            if (neighbour_chunk && neighbour_chunk.value().get().has(_layer)) {
                ivec2 other_chunk_nbr_pos = nbr_pos + bounds_signs * -(int)SIZE_TILES;
                connects = should_connect(tile_id, neighbour_chunk.value().get().at(_layer).tile_at(other_chunk_nbr_pos));
            }
        }
        connects_in_dir[dir_to_index.at(nbr_dir)] = connects;
    }}

    return connects_in_dir;
}

void Chunk::Layer::replace_tiles(const arr<id, SIZE_TILES * SIZE_TILES>& arr) {
    _tiles = arr; for (uint i = 0; i < SIZE_TILES * SIZE_TILES; ++i) update_shape(to_pos(i));
}

void Chunk::Layer::update_shape(const uvec2& pos) {
    assert(is_in_range(pos));
    if (auto opt = DataManager::get_tile(tile_at(pos))) {
        auto& handle = opt.value().get();    
        if (handle.model_type() != data::TileHandle::ModelType::Block) return;
    }
    else return print<warning, Chunk>("Could not get tile handle for '{}'.", tile_at(pos));
    
    _shapes[to_index(pos)] = tile_impl::from_connections(find_connections(pos));
}