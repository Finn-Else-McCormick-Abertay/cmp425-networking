#pragma once

#include <array>
#include <map>
#include <functional>
#include <util/vec.h>
#include <terrain/tile.h>

#include <glaze/glaze.hpp>
#include <util/std_aliases.h>

class Chunk {
public:
    static constexpr uint SIZE_TILES = 16;

    Chunk(); Chunk(const Chunk&); Chunk(Chunk&&);
    const ivec2& get_coords() const;

    Tile* tile_at(const uvec2& pos);
    const Tile* tile_at(const uvec2& pos) const;
    bool set_tile_at(const uvec2& pos, Tile, bool update = true);

    bool empty() const;

    Chunk& operator=(const Chunk&);
    Chunk& operator=(Chunk&&);

private:
    friend class World; friend class glz::meta<Chunk>; friend class glz::meta<World>;
    ivec2 _chunk_coords; std::map<ivec2, Chunk*> _neighbours; // Set by containing world
    Chunk* get_chunk_neighbour(const ivec2&);
    void set_chunk_neighbour(const ivec2&, Chunk*, bool recursive = true);
    void update_shapes_along_neighbour_border(const ivec2&, bool neighbour_only = false);

    static bool is_in_range(const uvec2& pos);
    static bool is_in_range(size_t);
    static size_t index_from_coords(const uvec2& pos);
    static uvec2 coords_from_index(size_t);

    using tile_array_flat = std::array<Tile, SIZE_TILES * SIZE_TILES>;
    tile_array_flat _tiles;

    std::array<data::id, SIZE_TILES * SIZE_TILES> get_tile_ids() const;
    void set_tiles_from_ids(const std::array<data::id, SIZE_TILES * SIZE_TILES>&);

    void update_shape_of(const uvec2& pos);

public:
    struct iterator {
        using difference_type = std::ptrdiff_t;
        using return_pair = std::pair<const uvec2, Tile*>;

        return_pair operator*() const;
        iterator& operator++();         iterator operator++(int);
        
        friend bool operator== (const iterator& a, const iterator& b);
        friend bool operator!= (const iterator& a, const iterator& b);
    private:
        tile_array_flat* _arr; size_t _i;
        iterator(tile_array_flat& array, size_t index);
        friend class Chunk;
    };

    struct const_iterator {
        using difference_type = std::ptrdiff_t;
        using return_pair = std::pair<const uvec2, const Tile*>;

        return_pair operator*() const;
        const_iterator& operator++();           const_iterator operator++(int);
        
        friend bool operator== (const const_iterator& a, const const_iterator& b);
        friend bool operator!= (const const_iterator& a, const const_iterator& b);
    private:
        const tile_array_flat* _arr; size_t _i;
        const_iterator(const tile_array_flat& array, size_t index);
        friend class Chunk;
    };

    iterator begin();   const_iterator cbegin() const;
    iterator end();     const_iterator cend() const;
};

template<> struct glz::meta<Chunk> {
    static constexpr auto set_chunk_coords = [](Chunk& chunk, const std::array<int, 2>& vals) {
        chunk._chunk_coords = ivec2(vals.at(0), vals.at(1));
    };
    static constexpr auto get_chunk_coords = [](const Chunk& chunk) {
        return std::array<int, 2> { chunk._chunk_coords.x, chunk._chunk_coords.y };
    };

    static constexpr auto value = object(
        "chunk_coords", custom<set_chunk_coords, get_chunk_coords>,
        "tiles", custom<&Chunk::set_tiles_from_ids, &Chunk::get_tile_ids>
    );
};