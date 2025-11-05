#include "chunk.h"
#include <cassert>
#include <iterator>
#include <map>
#include <vector>
#include <util/prelude.h>
#include <util/console.h>

#include <data/definitions.h>
#include <data/data_manager.h>

using namespace std;

Chunk::Chunk() : _tiles{} {}

const ivec2& Chunk::get_coords() const { return _chunk_coords; }

bool Chunk::is_in_range(const uvec2& pos) { return pos.x < SIZE_TILES && pos.y < SIZE_TILES; }
bool Chunk::is_in_range(size_t index) { return index < SIZE_TILES * SIZE_TILES; }

size_t Chunk::index_from_coords(const uvec2& pos) {
    assert(is_in_range(pos));
    return pos.y * SIZE_TILES + pos.x; 
}

uvec2 Chunk::coords_from_index(size_t index) {
    assert(is_in_range(index));
    return uvec2( index % SIZE_TILES, index / SIZE_TILES );
}

Tile* Chunk::tile_at(const uvec2& pos) { return is_in_range(pos) ? &_tiles.at(index_from_coords(pos)) : nullptr; }
const Tile* Chunk::tile_at(const uvec2& pos) const { return is_in_range(pos) ? &_tiles.at(index_from_coords(pos)) : nullptr; }
bool Chunk::set_tile_at(const uvec2& pos, Tile tile) {
    if (!is_in_range(pos)) return false;
    _tiles[index_from_coords(pos)] = tile;

    // Update shapes
    for (int i = -1; i <= 1; ++i) { for (int j = -1; j <= 1; ++j) {
        ivec2 nbr_pos = ivec2(i, j) + pos;
        if (nbr_pos.x < 0 || nbr_pos.y < 0 || nbr_pos.x >= SIZE_TILES || nbr_pos.y >= SIZE_TILES) {
            if (auto neighbour_chunk = get_chunk_neighbour(ivec2(
                nbr_pos.x < 0 ? -1 : nbr_pos.x >= SIZE_TILES ? 1 : 0,
                nbr_pos.y < 0 ? -1 : nbr_pos.y >= SIZE_TILES ? 1 : 0
            ))) {
                ivec2 other_chunk_nbr_pos = ivec2(
                    nbr_pos.x < 0 ? nbr_pos.x + SIZE_TILES : nbr_pos.x >= SIZE_TILES ? nbr_pos.x - SIZE_TILES : nbr_pos.x,
                    nbr_pos.y < 0 ? nbr_pos.y + SIZE_TILES : nbr_pos.y >= SIZE_TILES ? nbr_pos.y - SIZE_TILES : nbr_pos.y
                );
                neighbour_chunk->update_shape_of(other_chunk_nbr_pos);
            }
        }
        else update_shape_of(nbr_pos);
    }}
    return true;
}

Chunk* Chunk::get_chunk_neighbour(const ivec2& dir) {
    assert(abs(dir.x) <= 1 && abs(dir.y) <= 1 && !(dir.x == 0 && dir.y == 0));
    return _neighbours.contains(dir) ? _neighbours.at(dir) : nullptr;
}

void Chunk::set_chunk_neighbour(const ivec2& dir, Chunk* neighbour, bool recursive) {
    assert(abs(dir.x) <= 1 && abs(dir.y) <= 1 && !(dir.x == 0 && dir.y == 0));
    _neighbours[dir] = neighbour;
    if (neighbour && recursive) neighbour->set_chunk_neighbour(-dir, this, false);
}

void Chunk::update_shape_of(const uvec2& pos) {
    Tile* tile = tile_at(pos);
    if (auto tile_def = data::Manager::get_tile(tile->type());
        tile_def && holds_alternative<data::DefaultModel>(tile_def->model)
        && get<data::DefaultModel>(tile_def->model) == data::DefaultModel::none
    ) return;

    map<ivec2, bool> should_connect_in_dir;
    for (int i = -1; i <= 1; ++i) { for (int j = -1; j <= 1; ++j) {
        if (i == 0 && j == 0) continue;
        bool should_connect = false;

        ivec2 nbr_dir = ivec2(i, j);
        ivec2 nbr_pos = nbr_dir + pos;
        if (nbr_pos.x < 0 || nbr_pos.y < 0 || nbr_pos.x >= SIZE_TILES || nbr_pos.y >= SIZE_TILES) {
            if (auto neighbour_chunk = get_chunk_neighbour(ivec2(
                nbr_pos.x < 0 ? -1 : nbr_pos.x >= SIZE_TILES ? 1 : 0,
                nbr_pos.y < 0 ? -1 : nbr_pos.y >= SIZE_TILES ? 1 : 0
            ))) {
                ivec2 other_chunk_nbr_pos = ivec2(
                    nbr_pos.x < 0 ? nbr_pos.x + SIZE_TILES : nbr_pos.x >= SIZE_TILES ? nbr_pos.x - SIZE_TILES : nbr_pos.x,
                    nbr_pos.y < 0 ? nbr_pos.y + SIZE_TILES : nbr_pos.y >= SIZE_TILES ? nbr_pos.y - SIZE_TILES : nbr_pos.y
                );
                should_connect = tile->should_connect_to(*neighbour_chunk->tile_at(other_chunk_nbr_pos));
            }
            else should_connect = false;
        }
        else { should_connect = tile->should_connect_to(*tile_at(nbr_pos)); }

        should_connect_in_dir[nbr_dir] = should_connect;
    }}

    bool open_l = should_connect_in_dir.at({-1, 0}), open_r = should_connect_in_dir.at({1, 0});
    bool open_t = should_connect_in_dir.at({0, -1}), open_b = should_connect_in_dir.at({0, 1});

    bool corner_lt = !should_connect_in_dir.at({-1, -1}), corner_rt = !should_connect_in_dir.at({1, -1});
    bool corner_lb = !should_connect_in_dir.at({-1, 1}), corner_rb = !should_connect_in_dir.at({1, 1});

    // This is deeply ugly but I can't think of a better way to do it
    Tile::Shape shape = Tile::Shape::SINGLE;
    if (open_l && open_r && open_t && open_b) {
        if (corner_lt && corner_lb && corner_rt && corner_rb) { shape = Tile::Shape::OPEN_LRTB_CORNER_LT_RT_LB_RB; }
        else if (corner_lt && corner_lb && corner_rt) { shape = Tile::Shape::OPEN_LRTB_CORNER_LT_LB_RT; }
        else if (corner_lt && corner_lb && corner_rb) { shape = Tile::Shape::OPEN_LRTB_CORNER_LT_LB_RB; }
        else if (corner_lt && corner_rt && corner_rb) { shape = Tile::Shape::OPEN_LRTB_CORNER_LT_RT_RB; }
        else if (corner_lb && corner_rt && corner_rb) { shape = Tile::Shape::OPEN_LRTB_CORNER_LB_RB_RT; }
        else if (corner_lt && corner_lb) { shape = Tile::Shape::OPEN_LRTB_CORNER_LT_LB; }
        else if (corner_lt && corner_rt) { shape = Tile::Shape::OPEN_LRTB_CORNER_LT_RT; }
        else if (corner_lt && corner_rb) { shape = Tile::Shape::OPEN_LRTB_CORNER_LT_RB; }
        else if (corner_lb && corner_rt) { shape = Tile::Shape::OPEN_LRTB_CORNER_LB_RT; }
        else if (corner_lb && corner_rb) { shape = Tile::Shape::OPEN_LRTB_CORNER_LB_RB; }
        else if (corner_rt && corner_rb) { shape = Tile::Shape::OPEN_LRTB_CORNER_RT_RB; }
        else if (corner_lt) { shape = Tile::Shape::OPEN_LRTB_CORNER_LT; }
        else if (corner_lb) { shape = Tile::Shape::OPEN_LRTB_CORNER_LB; }
        else if (corner_rt) { shape = Tile::Shape::OPEN_LRTB_CORNER_RT; }
        else if (corner_rb) { shape = Tile::Shape::OPEN_LRTB_CORNER_RB; }
        else { shape = Tile::Shape::OPEN_LRTB; }
    }
    else if (open_l && open_r && open_t) {
        if (corner_lt && corner_rt) { shape = Tile::Shape::OPEN_LRT_CORNER_LT_RT; }
        else if (corner_lt) { shape = Tile::Shape::OPEN_LRT_CORNER_LT; }
        else if (corner_rt) { shape = Tile::Shape::OPEN_LRT_CORNER_RT; }
        else { shape = Tile::Shape::OPEN_LRT; }
    }
    else if (open_l && open_r && open_b) {
        if (corner_lb && corner_rb) { shape = Tile::Shape::OPEN_LRB_CORNER_LB_RB; }
        else if (corner_lb) { shape = Tile::Shape::OPEN_LRB_CORNER_LB; }
        else if (corner_rb) { shape = Tile::Shape::OPEN_LRB_CORNER_RB; }
        else { shape = Tile::Shape::OPEN_LRB; }
    }
    else if (open_l && open_t && open_b) {
        if (corner_lt && corner_lb) { shape = Tile::Shape::OPEN_LTB_CORNER_LT_LB; }
        else if (corner_lt) { shape = Tile::Shape::OPEN_LTB_CORNER_LT; }
        else if (corner_lb) { shape = Tile::Shape::OPEN_LTB_CORNER_LB; }
        else { shape = Tile::Shape::OPEN_LTB; }
    }
    else if (open_r && open_t && open_b) {
        if (corner_rt && corner_rb) { shape = Tile::Shape::OPEN_RTB_CORNER_RT_RB; }
        else if (corner_rt) { shape = Tile::Shape::OPEN_RTB_CORNER_RT; }
        else if (corner_rb) { shape = Tile::Shape::OPEN_RTB_CORNER_RB; }
        else { shape = Tile::Shape::OPEN_RTB; }
    }
    else if (open_l && open_t) {
        if (corner_lt) { shape = Tile::Shape::OPEN_LT_CORNER_LT; }
        else { shape = Tile::Shape::OPEN_LT; }
    }
    else if (open_l && open_b) {
        if (corner_lb) { shape = Tile::Shape::OPEN_LB_CORNER_LB; }
        else { shape = Tile::Shape::OPEN_LB; }
    }
    else if (open_r && open_t) {
        if (corner_rt) { shape = Tile::Shape::OPEN_RT_CORNER_RT; }
        else { shape = Tile::Shape::OPEN_RT; }
    }
    else if (open_r && open_b) {
        if (corner_rb) { shape = Tile::Shape::OPEN_RB_CORNER_RB; }
        else { shape = Tile::Shape::OPEN_RB; }
    }
    else if (open_l && open_r) { shape = Tile::Shape::OPEN_LR; }
    else if (open_t && open_b) { shape = Tile::Shape::OPEN_TB; }
    else if (open_l) { shape = Tile::Shape::OPEN_L; }
    else if (open_r) { shape = Tile::Shape::OPEN_R; }
    else if (open_t) { shape = Tile::Shape::OPEN_T; }
    else if (open_b) { shape = Tile::Shape::OPEN_B; }
    tile->set_shape(shape);
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