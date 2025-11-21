#pragma once

#include <functional>
#include <ranges>
#include <terrain/tile_shape.h>
#include <data/handle/tile.h>
#include <data/namespaced_id.h>
#include <render/drawable.h>
#include <alias/bitset.h>

#include <glaze/glaze.hpp>
#include <prelude.h>
#include <prelude/vec.h>
#include <prelude/opt.h>
#include <prelude/containers.h>

static constexpr uint TILE_SIZE = 8;
class Chunk {
public:  static constexpr uint SIZE_TILES = 16;
private: static constexpr uint TILE_COUNT = SIZE_TILES * SIZE_TILES;
public:
    Chunk(); Chunk(const Chunk&); Chunk(Chunk&&);
    const ivec2& get_coords() const;

    class Layer {
    public:
        Layer(Chunk* = nullptr, draw_layer = 0);

        const id& tile_at(const uvec2& pos) const;
        bool set_tile_at(const uvec2& pos, const id&);

        TileShape shape_at(const uvec2& pos) const;
        
        bitset<8> find_connections_at(const uvec2& pos) const;

        bool empty() const;
    private:
        friend class World; friend class Chunk;
        friend class glz::meta<Chunk::Layer>; friend class glz::meta<Chunk>;
        void update_shape_of(const uvec2& pos);

        arr<id, TILE_COUNT> _tiles;
        arr<TileShape, TILE_COUNT> _shapes;

        draw_layer _layer;
        Chunk* _owner;
    };

    Layer& operator[](draw_layer layer); const Layer& operator[](draw_layer layer) const;
    Layer& at(draw_layer layer); const Layer& at(draw_layer layer) const;
    bool has(draw_layer layer) const;

    inline auto layers() const { return _layers | std::views::keys; }

    bool empty() const;

    Chunk& operator=(const Chunk&);
    Chunk& operator=(Chunk&&);

private:
    ivec2 _chunk_coords; bstmap<ivec2, Chunk*> _neighbours; // Set by containing world
    Chunk* get_chunk_neighbour(const ivec2&);
    void set_chunk_neighbour(const ivec2&, Chunk*, bool recursive = true);

    void update_all_layer_shapes_at(const uvec2& pos);
    void update_shapes_along_neighbour_border(const ivec2&, bool neighbour_only = false);
    
    static bool is_in_range(const uvec2& pos); 
    static bool is_in_range(size_t);
    static size_t index_from_coords(const uvec2& pos);
    static uvec2 coords_from_index(size_t);

    friend class World;
    friend class glz::meta<Chunk>; friend class glz::meta<Chunk::Layer>; friend class glz::meta<World>;

    bstmap<draw_layer, Layer> _layers;
};

template<> struct glz::meta<Chunk::Layer> {
    static constexpr auto set_tiles = [](Chunk::Layer& layer, const ::arr<id, Chunk::TILE_COUNT>& vals) {
        layer._tiles = vals;
        for (uint i = 0; i < Chunk::TILE_COUNT; ++i) layer.update_shape_of(Chunk::coords_from_index(i));
    };
    static constexpr auto value { custom<set_tiles, &Chunk::Layer::_tiles> };
};

template<> struct glz::meta<Chunk> {
    static constexpr auto set_chunk_coords = [](Chunk& chunk, const ::arr<int, 2>& vals) {
        chunk._chunk_coords = ivec2(vals.at(0), vals.at(1));
    };
    static constexpr auto get_chunk_coords = [](const Chunk& chunk) {
        return ::arr<int, 2> { chunk._chunk_coords.x, chunk._chunk_coords.y };
    };

    static constexpr auto set_layers = [](Chunk& chunk, const bstmap<draw_layer, Chunk::Layer>& layers) {
        chunk._layers = layers;
        for (auto& [i, layer] : chunk._layers) layer._owner = &chunk;
    };

    static constexpr auto value = object(
        "chunk_coords", custom<set_chunk_coords, get_chunk_coords>,
        "layers", custom<set_layers, &Chunk::_layers>
    );
};