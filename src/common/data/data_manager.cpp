#include "data_manager.h"

#include <util/console.h>
#include <filesystem>
#include <fstream>
#include <sstream>

#include <util/glaze_prelude.h>
#include <glaze/json.hpp>

using namespace std;

DEFINE_SINGLETON(data::Manager);

void data::Manager::reload() {
    auto data_root = filesystem::relative("resources/data");
    for (auto& entry : filesystem::directory_iterator(data_root)) {
        if (entry.is_directory()) load_namespace(entry);
    }
    print<success, Manager>("Data reloaded.");
}

void data::Manager::load_namespace(const std::filesystem::path& root) {
    std::string namespace_id = root.filename().generic_string();
    inst()._namespaces.erase(namespace_id);

    for (auto file : filesystem::directory_iterator(root)) {
        if (file.path().extension().generic_string() == ".json") additive_load_file(namespace_id, file.path());
    }
    print<success, Manager>("Loaded namespace '{}'.", namespace_id);
}

void data::Manager::additive_load_file(const string& nmspace, const filesystem::path& filepath) {
    auto file = ifstream(filepath);
    if (!file) return;
    
    stringstream buffer;
    buffer << file.rdbuf();

    auto result = glz::read_json<Registry>(buffer.str());
    if (!result) return print<error, Manager>("Error loading from '{}' : {} ({}).", filepath, result.error().custom_error_message, (int)result.error().ec);

    Registry reg = result.value();
    
    if (!inst()._namespaces.contains(nmspace)) inst()._namespaces.emplace(nmspace, reg);
    else {
        auto& existing_reg = inst()._namespaces.at(nmspace);
        for (auto& [name, tile] : reg.tiles) existing_reg.tiles[name] = tile;
        for (auto& [name, item] : reg.items) existing_reg.items[name] = item;
    }
}

const data::Manager::Registry* data::Manager::get_namespace(const std::string& nmspace) {
    if (inst()._namespaces.contains(nmspace)) return &inst()._namespaces.at(nmspace);
    else print<warning, Manager>("No such namespace '{}'.", nmspace);
    return nullptr;
}

const data::Tile* data::Manager::get_tile(const id& id) {
    if (auto reg = get_namespace(id.nmspace()); reg && reg->tiles.contains(id.name())) return &reg->tiles.at(id.name());
    else print<warning, Manager>("Namespace '{}' contains no such tile '{}'.", id.nmspace(), id.name());
    return nullptr;
}

const data::Item* data::Manager::get_item(const id& id) {
    if (auto reg = get_namespace(id.nmspace()); reg && reg->items.contains(id.name())) return &reg->items.at(id.name());
    else print<warning, Manager>("Namespace '{}' contains no such item '{}'.", id.nmspace(), id.name());
    return nullptr;
}