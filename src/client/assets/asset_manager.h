#pragma once

#include <util/helper/singleton.h>

#include <data/namespaced_id.h>
#include <alias/SFML/graphics.h>

#include <prelude.h>
#include <prelude/opt.h>
#include <prelude/containers.h>
#include <prelude/filesystem.h>

namespace assets {
    class Manager { DECL_SINGLETON(Manager);
    public:
        static void reload();
        static void on_data_changed();
        
        static const Texture& get_tile_texture(const data::id&);
        static const Texture& get_item_texture(const data::id&);

        static opt_cref<Texture> try_get_tile_texture(const data::id&);
        static opt_cref<Texture> try_get_item_texture(const data::id&);

    private:
        static opt<Texture> attempt_load_texture(const filepath&);

        Texture _placeholder_texture;
        hashmap<data::id, Texture> _tile_textures;
        hashmap<data::id, Texture> _item_textures;
    };
}