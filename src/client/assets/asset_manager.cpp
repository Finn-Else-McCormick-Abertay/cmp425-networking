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

const sf::Texture& assets::Manager::get_tile_texture(const data::id& id) {
    if (auto opt = try_get_tile_texture(id)) return opt.value();
    return inst()._placeholder_texture;
}
const sf::Texture& assets::Manager::get_item_texture(const data::id& id) {
    if (auto opt = try_get_item_texture(id)) return opt.value();
    return inst()._placeholder_texture;
}

opt_cref<sf::Texture> assets::Manager::try_get_tile_texture(const data::id& id) {
    if (inst()._tile_textures.contains(id)) return cref(inst()._tile_textures.at(id));
    return nullopt;
}
opt_cref<sf::Texture> assets::Manager::try_get_item_texture(const data::id& id) {
    if (inst()._item_textures.contains(id)) return cref(inst()._item_textures.at(id));
    return nullopt;
}

void assets::Manager::on_data_changed() {
    if (auto opt = attempt_load_texture("placeholder"); opt) inst()._placeholder_texture = move(opt.value());
    else inst()._placeholder_texture = sf::Texture();

    for (auto& id : data::Manager::tile_ids()) {
        auto tile = data::Manager::get_tile(id);
        filesystem::path logical_path = tile.value().get()._texture_path;
        if (!logical_path.empty()) {
            auto opt = attempt_load_texture(logical_path);
            if (opt) inst()._tile_textures[id] = move(opt.value());
        }
    }
    
    for (auto& id : data::Manager::item_ids()) {
        auto item = data::Manager::get_item(id);
        filesystem::path logical_path = item.value().get()._texture_path;
        if (!logical_path.empty()) {
            auto opt = attempt_load_texture(logical_path);
            if (opt) inst()._item_textures[id] = move(opt.value());
        }
    }
}

opt<sf::Texture> assets::Manager::attempt_load_texture(const filesystem::path& path) {
    auto full_path = filesystem::relative("resources/assets/textures/") / path;
    full_path = full_path.replace_extension(".png").lexically_normal();
    
    if (!filesystem::exists(full_path)) { print<error, Manager>("No such texture '{}'.", full_path); return nullopt; }
    sf::Texture texture; if (!texture.loadFromFile(full_path)) { print<error, Manager>("Failed to load texture from '{}'.", full_path); return nullopt; }

    print<info, Manager>("Loaded texture from '{}'.", full_path); return texture;
}