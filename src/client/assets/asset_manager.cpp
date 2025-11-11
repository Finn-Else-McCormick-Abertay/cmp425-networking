#include "asset_manager.h"

#include <data/data_manager.h>
#include <util/console.h>

using namespace std;

DEFINE_SINGLETON(assets::Manager);

void assets::Manager::reload() {
    inst()._tile_textures.clear();
    inst()._item_textures.clear();
    on_data_changed();
}

sf::Texture* assets::Manager::get_tile_texture(const data::id& id) { if (inst()._tile_textures.contains(id)) return &inst()._tile_textures.at(id); return nullptr; }
sf::Texture* assets::Manager::get_item_texture(const data::id& id) { if (inst()._item_textures.contains(id)) return &inst()._item_textures.at(id); return nullptr; }

void assets::Manager::on_data_changed() {
    for (auto& id : data::Manager::tile_ids()) {
        auto tile = data::Manager::get_tile(id);
        filesystem::path logical_path = tile.value().get()._texture_path;
        if (!logical_path.empty()) attempt_load_texture_to(inst()._tile_textures, id, logical_path);
    }
    
    for (auto& id : data::Manager::item_ids()) {
        auto item = data::Manager::get_item(id);
        filesystem::path logical_path = item.value().get()._texture_path;
        if (!logical_path.empty()) attempt_load_texture_to(inst()._item_textures, id, logical_path);
    }
}

void assets::Manager::attempt_load_texture_to(map<data::id, sf::Texture>& map, const data::id& id, const filesystem::path& path) {
    auto full_path = filesystem::relative("resources/assets/textures/") / path;
    full_path = full_path.replace_extension(".png").lexically_normal();
    
    if (!filesystem::exists(full_path)) return print<error, Manager>("No such texture '{}'.", full_path);
    
    sf::Texture texture;
    if (!texture.loadFromFile(full_path)) return print<error, Manager>("Failed to load texture from '{}'.", full_path);

    map[id] = move(texture);
    print<info, Manager>("Loaded texture from '{}'.", full_path);
}