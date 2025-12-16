#include "asset_manager.h"

#include <data/data_manager.h>
#include <console.h>

DEFINE_SINGLETON(AssetManager);

void AssetManager::reload() {
    unload();
    on_data_changed();
}

void AssetManager::unload() {
    inst()._textures.clear();
    inst()._fonts.clear();
}

opt_cref<Texture> AssetManager::try_get_texture(const asset_id& id) {
    if (id.type() == AssetType::unknown) return try_get_texture(id.with_type(AssetType::texture));
    if (inst()._textures.contains(id)) return cref(inst()._textures.at(id));
    return nullopt;
}
const Texture& AssetManager::get_texture(const asset_id& id) {
    if (auto opt = try_get_texture(id)) return *opt;
    if (auto opt = try_get_texture(asset_id("default::placeholder"_id, id.type()))) return *opt;
    if (auto opt = try_get_texture(asset_id("default::placeholder"_id, AssetType::texture))) return *opt;
    print<error, AssetManager>("Failed to find texture placeholder.");
    throw std::exception();
}

opt_cref<Font> AssetManager::try_get_font(const asset_id& id) {
    if (id.type() == AssetType::unknown) return try_get_font(id.with_type(AssetType::font));
    if (inst()._fonts.contains(id)) return cref(inst()._fonts.at(id));
    return nullopt;
}
const Font& AssetManager::get_font(const asset_id& id) {
    if (auto opt = try_get_font(id)) return *opt;
    if (auto opt = try_get_font(asset_id("default::placeholder"_id, id.type()))) return *opt;
    if (auto opt = try_get_font(asset_id("default::placeholder"_id, AssetType::font))) return *opt;
    print<error, AssetManager>("Failed to find font placeholder.");
    throw std::exception();
}

void AssetManager::on_data_changed() {
    // Load or create placeholder texture
    if (!load(asset_id("default::placeholder"_id, AssetType::texture)))
        inst()._textures[asset_id("default::placeholder"_id, AssetType::texture)] = Texture();

    // Load tile textures
    for (auto& id : DataManager::tile_ids()) {
        const data::TileHandle& handle = *DataManager::get_tile(id);
        if (handle.model_type() == data::TileHandle::ModelType::Block) load(asset_id(id, AssetType::texture_tileset), handle._texture_path);
        else if (handle.model_type() == data::TileHandle::ModelType::Custom) load(asset_id(id, AssetType::texture), handle._texture_path);
    }
    
    // Load item textures
    for (auto& id : DataManager::item_ids()) {
        const data::ItemHandle& handle = *DataManager::get_item(id);
        load(asset_id(id, AssetType::texture_item), handle._texture_path);
    }

    // Load fonts
    load_all_within("font", AssetType::font);
    auto font_folder = DataManager::resources_folder() / "assets" / "font";
}

fs::path AssetManager::validate_file_path(const fs::path& input_path, AssetType type) {
    auto path = (DataManager::resources_folder() / "assets" / input_path).lexically_normal();
    if (type == AssetType::unknown) return path;
    dyn_arr<str> allowed_exts;
    switch (type) {
        case AssetType::font:
            allowed_exts = { ".ttf", ".otf" }; break;
        case AssetType::texture: case AssetType::texture_item: case AssetType::texture_tileset:
            allowed_exts = { ".png", ".jpg", ".jpeg", ".bmp", ".tga", ".hdr" }; break;
        default: return path;
    }
    if (!path.has_extension()) {
        for (auto& ext : allowed_exts) {
            auto path_with_ext = path.replace_extension(ext);
            if (fs::exists(path_with_ext)) { path = path_with_ext; break; }
        }
    }
    else if (!ranges::contains(allowed_exts, path.extension()))
        print<warning, AssetManager>("{} : invalid file extension '{}' for {}.", "assets" / input_path, path.extension(), type);
    
    return path;
}

bool AssetManager::load(const asset_id& asset_id, opt<fs::path> path_opt) {
    fs::path path = *path_opt.or_else([&asset_id](){
        // TK - factor in namespaces
        fs::path type_path;
        switch (asset_id.type()) {
            case AssetType::font: type_path = "font"; break;
            case AssetType::texture: type_path = "texture"; break;
            case AssetType::texture_tileset: type_path = "texture/tileset"; break;
            case AssetType::texture_item: type_path = "texture/item"; break;
            default: type_path = ""; break;
        }
        return make_opt(type_path / asset_id.name());
    });
    path = validate_file_path(path, asset_id.type());
    if (!fs::exists(path)) { print<error, AssetManager>("Could not find asset {} at {}.", asset_id, path); return false; }

    switch (asset_id.type()) {
        case AssetType::font: {
            Font font; font.setSmooth(false);
            if (!font.openFromFile(path)) { print<error, AssetManager>("Failed to load font from '{}'.", path); return false; }
            inst()._fonts[asset_id] = move(font);
            return true;
        } break;
        case AssetType::texture: case AssetType::texture_tileset: case AssetType::texture_item: {
            Texture texture;
            if (!texture.loadFromFile(path)) { print<error, AssetManager>("Failed to load texture from '{}'.", path); return false; }
            inst()._textures[asset_id] = move(texture);
            return true;
        } break;
        default: {
            print<error, AssetManager>("Could not load {} - unhandled asset type.", asset_id);
            return false;
        } break;
    }
    return false;
}

void AssetManager::load_all_within(const fs::path& folder, AssetType type) {
    auto asset_root = (DataManager::resources_folder() / "assets").lexically_normal();
    auto folder_root = (asset_root / folder).lexically_normal();
    for (auto& entry : fs::dir_recursive(folder_root)) {
        if (entry.is_directory()) continue;
        asset_id item_id = asset_id(id("default", fs::proximate(entry.path(), folder_root).replace_extension("").generic_string()), type);
        load(item_id, fs::proximate(entry.path(), asset_root));
    }
}