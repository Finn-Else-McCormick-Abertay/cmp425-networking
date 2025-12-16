#pragma once

#include <util/helper/singleton.h>

#include <assets/asset_id.h>
#include <alias/SFML/graphics.h>

#include <prelude.h>
#include <prelude/opt.h>
#include <prelude/containers.h>
#include <prelude/filesystem.h>
#include <alias/ranges.h>

class DataManager;

class AssetManager { DECL_SINGLETON(AssetManager);
public:
    static void reload();
    static void unload();

    static const Texture& get_texture(const asset_id&); static opt_cref<Texture> try_get_texture(const asset_id&);
    static const Font& get_font(const asset_id&);       static opt_cref<Font> try_get_font(const asset_id&);

private:
    static void on_data_changed(); friend class DataManager;

    static bool load(const asset_id&, opt<fs::path> path = nullopt);
    static void load_all_within(const fs::path& folder, AssetType type);
    
    static fs::path validate_file_path(const fs::path&, AssetType type);

    hashmap<asset_id, Texture> _textures;
    hashmap<asset_id, Font> _fonts;
};