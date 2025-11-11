#pragma once

#include <terrain/chunk.h>
#include <optional>
#include <map>
#include <vector>

#include <glaze/glaze.hpp>
#include <util/std_aliases.h>

class World {
public:
    World();

    Chunk* chunk_at(const ivec2& chunk_coords);
    const Chunk* chunk_at(const ivec2& chunk_coords) const;
    Chunk* set_chunk(const ivec2& chunk_coords, std::optional<Chunk>&&, bool replace = true);

    Chunk* get_or_make_chunk_at(const ivec2& chunk_coords);

private:
    friend class glz::meta<World>;
    std::map<ivec2, Chunk> _chunk_map;

    std::vector<Chunk> get_flattened_chunks() const;
    void set_chunks_from_flattened(const std::vector<Chunk>&);

public:
    struct iterator {
        using element_type = Chunk;     using difference_type = std::ptrdiff_t;
        using pointer = element_type*;  using reference = element_type&;

        reference operator*() const;    pointer operator->();
        iterator& operator++();         iterator operator++(int);

        friend bool operator== (const iterator& a, const iterator& b);
        friend bool operator!= (const iterator& a, const iterator& b);
    private:
        std::map<ivec2, Chunk>::iterator _internal_it;
        iterator(std::map<ivec2, Chunk>::iterator);
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
        std::map<ivec2, Chunk>::const_iterator _internal_it;
        const_iterator(std::map<ivec2, Chunk>::const_iterator);
        friend class World;
    };

    iterator begin();
    iterator end();
    const_iterator cbegin() const;
    const_iterator cend() const;
};

template<> struct glz::meta<World> {
    static constexpr auto value = object(
        "chunks", glz::custom<&World::set_chunks_from_flattened, &World::get_flattened_chunks>
    );
};