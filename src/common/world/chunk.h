#pragma once

#include <functional>
#include <ranges>
#include <world/tile_shape.h>
#include <data/handle/tile.h>
#include <data/namespaced_id.h>
#include <render/drawable.h>
#include <alias/bitset.h>

#include <glaze/glaze.hpp>
#include <prelude.h>
#include <prelude/vec.h>
#include <prelude/opt.h>
#include <prelude/containers.h>
#include <alias/ranges.h>

#include <world/tile_layer.h>

class Level;
class World;

static constexpr uint TILE_SIZE = 8;
class Chunk {
public:
    static constexpr uint SIZE_TILES = 16; class Layer;

    Chunk(); Chunk(const Chunk&); Chunk(Chunk&&);
    Chunk& operator=(const Chunk&); Chunk& operator=(Chunk&&);
    
    bool has(tile_layer layer) const;
    const Layer& operator[](tile_layer layer) const; Layer& operator[](tile_layer layer);
    const Layer& at(tile_layer layer) const;         Layer& at(tile_layer layer);
    bool empty() const;

    inline auto layers() const { return _layers | views::keys; }
    
    const ivec2& pos() const;

    class Layer {
    public:
        const id& tile_at(const uvec2& pos) const;
        bool set_tile_at(const uvec2& pos, const id&);
        TileShape shape_at(const uvec2& pos) const;
        bool empty() const;

        Layer(Chunk* = nullptr, tile_layer = tile_layer::Foreground);
    private:
        arr<id, SIZE_TILES * SIZE_TILES> _tiles;
        arr<TileShape, SIZE_TILES * SIZE_TILES> _shapes;
        tile_layer _layer; Chunk* _owner;

        void update_shape(const uvec2& pos); void replace_tiles(const arr<id, SIZE_TILES * SIZE_TILES>&);
        bitset<8> find_connections(const uvec2& pos) const;

        friend class Level; friend class Chunk;
        friend class glz::meta<Chunk::Layer>; friend class glz::meta<Chunk>;
    };
private:
    bstmap<tile_layer, Layer> _layers;
    void replace_layers(const bstmap<tile_layer, Layer>&); void post_update_layers();

    mutable bool _updated_since_sync = false;
    ivec2 _pos; bstmap<ivec2, Chunk*> _neighbours;
    opt_ref<Chunk> get_chunk_neighbour(const ivec2&);
    void set_chunk_neighbour(const ivec2&, opt_ref<Chunk>, bool recursive = true);

    void update_all_layer_shapes_at(const uvec2& pos);
    void update_shapes_along_neighbour_border(const ivec2&, bool neighbour_only = false);
    
    static bool is_in_range(const uvec2& pos); static bool is_in_range(size_t);
    static size_t to_index(const uvec2& pos);
    static uvec2 to_pos(size_t);

    friend class ::Level;
    friend class ::World;
    friend class glz::meta<Chunk>;
    friend class glz::meta<Chunk::Layer>;
    friend class glz::meta<::Level>;
};

template<> struct glz::meta<Chunk::Layer> { static constexpr auto value { custom<&Chunk::Layer::replace_tiles, &Chunk::Layer::_tiles> }; };
template<> struct glz::meta<Chunk> { static constexpr auto value = object("layers", custom<&Chunk::replace_layers, &Chunk::_layers>); };