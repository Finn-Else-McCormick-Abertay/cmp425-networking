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
        filesystem::path texture_logical_path;
        if (tile->texture) texture_logical_path = tile->texture.value();
        else if (holds_alternative<data::DefaultModel>(tile->model)) {
            auto default_model = get<data::DefaultModel>(tile->model);
            if (default_model == data::DefaultModel::block) {
                texture_logical_path = "tileset/" + id.name();
            }
        }
        else { /* Handle manual models */ }

        if (texture_logical_path.empty()) continue;

        auto filepath = filesystem::relative("resources/assets/textures/") / texture_logical_path.replace_extension(".png").lexically_normal();
        if (filesystem::exists(filepath)) {
            sf::Texture texture; texture.loadFromFile(filepath);
            inst()._tile_textures[id] = move(texture);
            print<info, Manager>("Loaded texture from '{}'.", filepath);
        }
        else print<error, Manager>("Could not load texture from '{}'.", filepath);
    }
    
    for (auto& id : data::Manager::item_ids()) {
        auto item = data::Manager::get_item(id);
        filesystem::path texture_logical_path;
        if (item->texture) texture_logical_path = item->texture.value();
        else texture_logical_path = "items/" + id.name();

        if (texture_logical_path.empty()) continue;

        auto filepath = filesystem::relative("resources/assets/textures/") / texture_logical_path.replace_extension(".png").lexically_normal();
        if (filesystem::exists(filepath)) {
            sf::Texture texture; texture.loadFromFile(filepath);
            inst()._item_textures[id] = move(texture);
            print<info, Manager>("Loaded texture from '{}'.", filepath);
        }
        else print<error, Manager>("Could not load texture from '{}'.", filepath);
    }
}