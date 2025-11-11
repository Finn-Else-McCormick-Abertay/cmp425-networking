#pragma once

#include <util/helper/singleton.h>

#include <SFML/Graphics/Texture.hpp>
#include <map>
#include <filesystem>
#include <data/namespaced_id.h>
#include <util/std_aliases.h>

namespace assets {
    class Manager { DECL_SINGLETON(Manager);
    public:
        static void reload();
        static void on_data_changed();
        
        static const sf::Texture& get_tile_texture(const data::id&);
        static const sf::Texture& get_item_texture(const data::id&);

        static opt_cref<sf::Texture> try_get_tile_texture(const data::id&);
        static opt_cref<sf::Texture> try_get_item_texture(const data::id&);

    private:
        static opt<sf::Texture> attempt_load_texture(const std::filesystem::path&);

        sf::Texture _placeholder_texture;
        std::map<data::id, sf::Texture> _tile_textures;
        std::map<data::id, sf::Texture> _item_textures;
    };
}