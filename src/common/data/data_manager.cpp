#include "data_manager.h"

#include <util/console.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include <util/glaze_prelude.h>
#include <glaze/json.hpp>

#ifdef CLIENT
#include <assets/asset_manager.h>
#endif

using namespace std;

DEFINE_SINGLETON(data::Manager);

namespace data_impl {
    struct Registry {
        std::unordered_map<str, data::definition::Tile> tiles;
        std::unordered_map<str, data::definition::Item> items;
    };
    static_assert(glz::reflectable<Registry>);

    Registry load_namespace(const str& namespace_id, const std::filesystem::path& root) {
        Registry existing;
        for (auto entry : filesystem::directory_iterator(root)) {
            if (entry.path().extension().generic_string() == ".json") {
                auto file = ifstream(entry.path());
                if (!file) continue;
                
                stringstream buffer;
                buffer << file.rdbuf();

                auto result = glz::read_json<Registry>(buffer.str());
                if (!result) {
                    print<error, data::Manager>("Error loading from '{}' : {} ({}).",
                        entry.path(), result.error().custom_error_message, (int)result.error().ec);
                    continue;
                }

                Registry reg = result.value();
                for (auto& [name, tile] : reg.tiles) existing.tiles[name] = tile;
                for (auto& [name, item] : reg.items) existing.items[name] = item;

            }
        }
        print<success, data::Manager>("Loaded namespace '{}'.", namespace_id);
        return existing;
    }
}

void data::Manager::reload() {
    inst()._tile_handles.clear();
    inst()._item_handles.clear();
    std::unordered_map<str, data_impl::Registry> namespaces;

    auto data_root = filesystem::relative("resources/data");
    for (auto& entry : filesystem::directory_iterator(data_root)) {
        if (entry.is_directory()) {
            str namespace_id = entry.path().filename().generic_string();
            namespaces[namespace_id] = data_impl::load_namespace(namespace_id, entry);
        }
    }

    for (auto& [nmspace, reg] : namespaces) {
        for (auto& [name, tile_def] : reg.tiles) {
            auto tile_id = id(nmspace, name);
            inst()._tile_handles.emplace(tile_id, TileHandle(tile_id, tile_def));
        }
        for (auto& [name, item_def] : reg.items) {
            auto item_id = id(nmspace, name);
            inst()._item_handles.emplace(item_id, ItemHandle(item_id, item_def));
        }
    }

    print<success, Manager>("Data reloaded.");
    #ifdef CLIENT
    assets::Manager::on_data_changed();
    #endif
}

opt_cref<data::TileHandle> data::Manager::get_tile(const id& id) {
    if (inst()._tile_handles.contains(id)) return cref(inst()._tile_handles.at(id));
    print<warning, Manager>("Namespace '{}' contains no such tile '{}'.", id.nmspace(), id.name());
    return nullopt;
}

opt_cref<data::ItemHandle> data::Manager::get_item(const id& id) {
    if (inst()._item_handles.contains(id)) return cref(inst()._item_handles.at(id));
    print<warning, Manager>("Namespace '{}' contains no such item '{}'.", id.nmspace(), id.name());
    return nullopt;
}