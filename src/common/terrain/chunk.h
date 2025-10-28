#pragma once

#include <array>
#include <functional>
#include <util/vec.h>
#include <terrain/tile.h>

class Chunk {
public:
    static constexpr uvec2 SIZE_TILES = { 16, 16 };

    Chunk();
    const ivec2& get_coords() const;

    Tile* tile_at(const uvec2& pos);
    const Tile* tile_at(const uvec2& pos) const;
    bool set_tile_at(const uvec2& pos, Tile);

private:
    ivec2 _chunk_coords; friend class World; // Set by the containing world

    static bool is_in_range(const uvec2& pos);
    static bool is_in_range(size_t);
    static size_t index_from_coords(const uvec2& pos);
    static uvec2 coords_from_index(size_t);

    using tile_array_flat = std::array<Tile, SIZE_TILES.x * SIZE_TILES.y>;
    tile_array_flat _tiles;

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
