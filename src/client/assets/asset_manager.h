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
        
        static const Texture& get_tile_texture(const id&);
        static const Texture& get_item_texture(const id&);

        static opt_cref<Font> get_font(const id&);

    private:
        static opt<Texture> attempt_load_texture(const filepath&);
        static opt<Font> attempt_load_font(const filepath&);

        Texture _placeholder_texture;
        hashmap<id, Texture> _tile_textures;
        hashmap<id, Texture> _item_textures;

        hashmap<id, Font> _fonts;
    };
}