#pragma once

#include <glaze/glaze.hpp>
#include <prelude.h>
#include <prelude/opt.h>
#include <prelude/containers.h>

#include <render/drawable.h>
#include <network/networked.h>
#include <terrain/chunk.h>
#include <data/namespaced_id.h>

#include <ranges>

class World : IDrawable, INetworked {
public:
    World(id, int32 seed);
    World(id = "default::world"_id);

    id type_id() const;
    int32 seed() const;

    Chunk* chunk_at(const ivec2& chunk_coords); const Chunk* chunk_at(const ivec2& chunk_coords) const;
    Chunk* set_chunk(const ivec2& chunk_coords, opt<Chunk>&&, bool replace = true);

    Chunk* get_or_make_chunk_at(const ivec2& chunk_coords);

    virtual dyn_arr<draw_layer> draw_layers() const override;
    #ifdef CLIENT
    virtual void draw(sf::RenderTarget&, draw_layer layer) override;
    #endif
    
    virtual dyn_arr<LogicalPacket> write_messages() const override;
    virtual void read_message(LogicalPacket&&) override;

private:
    id _world_type_id;
    int32 _world_seed;

    friend class glz::meta<World>;
    bstmap<ivec2, Chunk> _chunk_map;

    dyn_arr<Chunk> get_flattened_chunks() const;
    void set_chunks_from_flattened(const dyn_arr<Chunk>&);

public:
    struct iterator {
        using element_type = Chunk;     using difference_type = std::ptrdiff_t;
        using pointer = element_type*;  using reference = element_type&;

        reference operator*() const;    pointer operator->();
        iterator& operator++();         iterator operator++(int);

        friend bool operator== (const iterator& a, const iterator& b);
        friend bool operator!= (const iterator& a, const iterator& b);
    private:
        bstmap<ivec2, Chunk>::iterator _internal_it;
        iterator(bstmap<ivec2, Chunk>::iterator);
        friend class World;
    };

    struct const_iterator {
        using element_type = Chunk;             using difference_type = std::ptrdiff_t;
        using pointer = const element_type*;    using reference = const element_type&;

        reference operator*() const;            pointer operator->();
        const_iterator& operator++();           const_iterator operator++(int);
        
        friend bool operator== (const const_iterator& a, const const_iterator& b);
        friend bool operator!= (const const_iterator& a, const const_iterator& b);
    private:
        bstmap<ivec2, Chunk>::const_iterator _internal_it;
        const_iterator(bstmap<ivec2, Chunk>::const_iterator);
        friend class World;
    };

    iterator begin();
    iterator end();
    const_iterator cbegin() const;
    const_iterator cend() const;
};

template<> struct glz::meta<World> {
    static constexpr auto value = object(
        "id", &World::_world_type_id,
        "seed", &World::_world_seed,
        "chunks", glz::custom<&World::set_chunks_from_flattened, &World::get_flattened_chunks>
    );
};