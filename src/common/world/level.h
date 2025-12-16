#pragma once

#include <glaze/glaze.hpp>
#include <prelude.h>
#include <prelude/opt.h>
#include <prelude/containers.h>

#include <render/drawable.h>
#include <network/networked.h>
#include <world/chunk.h>
#include <data/namespaced_id.h>

#include <alias/ranges.h>

class Level : IDrawable {
public:
    opt_ref<Chunk> chunk_at(const ivec2& chunk_coords);
    opt_cref<Chunk> chunk_at(const ivec2& chunk_coords) const;
    opt_ref<Chunk> set_chunk(const ivec2& chunk_coords, opt<Chunk>&&, bool replace = true);

    Chunk& get_or_make_chunk_at(const ivec2& chunk_coords);

    inline auto chunks() { return _chunk_map | views::values; }
    inline auto chunks() const { return _chunk_map | views::values; }

private:
    bstmap<ivec2, Chunk> _chunk_map;
    friend class glz::meta<Level>;
    
    // Rendering logic
    #ifdef CLIENT
    virtual dyn_arr<draw_layer> draw_layers() const override;
    virtual void draw(sf::RenderTarget&, draw_layer layer) override;
    #endif
};

template<> struct glz::meta<Level> {
    static constexpr auto write_chunks = [](const Level& level) -> bstmap<str, Chunk> {
        bstmap<str, Chunk> chunks;
        for (auto& [pos, chunk] : level._chunk_map)
            chunks[fmt::format("{}", fmt::join(pos, ","))] = chunk;
        return chunks;
    };

    static constexpr auto read_chunks = [](Level& level, const bstmap<str, Chunk>& chunks) {
        for (auto& [key, chunk] : chunks) {
            auto numbers = key | views::split(str(",")) | ranges::to<dyn_arr<str>>();
            assert(numbers.size() == 2);
            ivec2 pos = ivec2(str_to<int>(numbers[0]), str_to<int>(numbers[1]));
            level.set_chunk(pos, chunk);
        }
    };

    static constexpr auto value = object(
        "chunks", glz::custom<read_chunks, write_chunks>
    );
};