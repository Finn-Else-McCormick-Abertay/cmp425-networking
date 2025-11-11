#pragma once

#include <util/helper/singleton.h>

#include <SFML/Graphics/Texture.hpp>
#include <map>
#include <filesystem>
#include <data/namespaced_id.h>

namespace assets {
    class Manager { DECL_SINGLETON(Manager);
    public:
        static void reload();
        static void on_data_changed();

        static sf::Texture* get_tile_texture(const data::id&);
        static sf::Texture* get_item_texture(const data::id&);

    private:
        static void attempt_load_texture_to(std::map<data::id, sf::Texture>&, const data::id&, const std::filesystem::path&);

        std::map<data::id, sf::Texture> _tile_textures;
        std::map<data::id, sf::Texture> _item_textures;
    };
}