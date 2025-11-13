#include "data_manager.h"

#include <console.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <set>

#include <glaze/glaze.hpp>
#include <glaze/json.hpp>

#ifdef CLIENT
#include <assets/asset_manager.h>
#endif

DEFINE_SINGLETON(data::Manager);

namespace data_impl {
    struct Registry {
        hashmap<str, data::definition::Tile> tiles;
        hashmap<str, data::definition::Item> items;
    };
    static_assert(glz::reflectable<Registry>);

    Registry load_namespace(const str& namespace_id, const filepath& root) {
        Registry existing;
        for (auto entry : dir(root)) {
            if (entry.path().extension().generic_string() == ".json") {
                auto file = std::ifstream(entry.path());
                if (!file) continue;
                
                std::stringstream buffer;
                buffer << file.rdbuf();

                auto result = glz::read_jsonc<Registry>(buffer.str());
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
    inst()._mapped_to_ids.clear(); inst()._ids_to_mapped.clear();
    hashmap<str, data_impl::Registry> namespaces;

    auto data_root = filesystem::relative("resources/data");
    for (auto& entry : dir(data_root)) {
        if (entry.is_directory()) {
            str namespace_id = entry.path().filename().generic_string();
            namespaces[namespace_id] = data_impl::load_namespace(namespace_id, entry);
        }
    }
    
    set<data::id> ordered_ids;

    for (auto& [nmspace, reg] : namespaces) {
        for (auto& [name, tile_def] : reg.tiles) {
            auto tile_id = id(nmspace, name);
            inst()._tile_handles.emplace(tile_id, TileHandle(tile_id, tile_def));
            ordered_ids.insert(tile_id);
        }
        for (auto& [name, item_def] : reg.items) {
            auto item_id = id(nmspace, name);
            inst()._item_handles.emplace(item_id, ItemHandle(item_id, item_def));
            ordered_ids.insert(item_id);
        }
    }

    // Setup mappings (this uses the set so they're assigned in alphabetical order rather than declaration order)
    uint32 used_mappings = 0;
    for (auto& id : ordered_ids) { inst().map_id(id, used_mappings); ++used_mappings; }

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

uint32 data::Manager::id_mapping(const data::id& id) { return inst()._ids_to_mapped.at(id); }

const data::id& data::Manager::mapped_id(uint32 mapped) { return inst()._mapped_to_ids.at(mapped); }

void data::Manager::map_id(const data::id& id, uint32 mapped) {
    _mapped_to_ids[mapped] = id;
    _ids_to_mapped[id] = mapped;
}