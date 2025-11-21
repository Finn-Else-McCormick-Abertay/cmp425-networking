#include "asset_manager.h"

#include <data/data_manager.h>
#include <console.h>


DEFINE_SINGLETON(assets::Manager);

void assets::Manager::reload() {
    inst()._tile_textures.clear();
    inst()._item_textures.clear();
    on_data_changed();
}

const Texture& assets::Manager::get_tile_texture(const id& id) {
    if (inst()._tile_textures.contains(id)) return inst()._tile_textures.at(id);
    return inst()._placeholder_texture;
}
const Texture& assets::Manager::get_item_texture(const id& id) {
    if (inst()._item_textures.contains(id)) return inst()._item_textures.at(id);
    return inst()._placeholder_texture;
}

opt_cref<Font> assets::Manager::get_font(const id& id) {
    if (inst()._fonts.contains(id)) return cref(inst()._fonts.at(id));
    print<warning, Manager>("No such font '{}'.", id);
    return nullopt;
}

void assets::Manager::on_data_changed() {
    // Load placeholder texture
    if (auto opt = attempt_load_texture("placeholder"); opt) inst()._placeholder_texture = move(opt.value());
    else inst()._placeholder_texture = Texture();

    // Load tile textures
    for (auto& id : data::Manager::tile_ids()) {
        auto tile = data::Manager::get_tile(id);
        filepath logical_path = tile.value().get()._texture_path;
        if (!logical_path.empty()) {
            auto opt = attempt_load_texture(logical_path);
            if (opt) inst()._tile_textures[id] = move(opt.value());
        }
    }
    
    // Load item textures
    for (auto& id : data::Manager::item_ids()) {
        auto item = data::Manager::get_item(id);
        filepath logical_path = item.value().get()._texture_path;
        if (!logical_path.empty()) {
            auto opt = attempt_load_texture(logical_path);
            if (opt) inst()._item_textures[id] = move(opt.value());
        }
    }

    // Load fonts
    for (auto& entry : dir("resources/assets/fonts/")) {
        if (entry.is_directory()) continue;
        auto opt = attempt_load_font(entry.path().filename());
        if (opt) {
            inst()._fonts[id(entry.path().stem().generic_string())] = move(opt.value());
            print<info, Manager>("Loaded font '{}'.", id(entry.path().stem().generic_string()));
        }
    }
}

opt<Texture> assets::Manager::attempt_load_texture(const filepath& path) {
    auto full_path = (filesystem::relative("resources/assets/textures/") / path).lexically_normal();
    if (!full_path.has_extension()) full_path = full_path.replace_extension(".png");
    
    if (!filesystem::exists(full_path)) { print<error, Manager>("No such texture '{}'.", full_path); return nullopt; }
    Texture texture; if (!texture.loadFromFile(full_path)) { print<error, Manager>("Failed to load texture from '{}'.", full_path); return nullopt; }
    return texture;
}

opt<Font> assets::Manager::attempt_load_font(const filepath& path) {
    auto full_path = (filesystem::relative("resources/assets/fonts/") / path).lexically_normal();
    if (!full_path.has_extension()) full_path = full_path.replace_extension(".ttf");
    
    if (!filesystem::exists(full_path)) { print<error, Manager>("No such font '{}'.", full_path); return nullopt; }
    Font font; if (!font.openFromFile(full_path)) { print<error, Manager>("Failed to load font from '{}'.", full_path); return nullopt; }
    font.setSmooth(false);
    return font;
}