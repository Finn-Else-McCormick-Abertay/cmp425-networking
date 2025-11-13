#include "save_manager.h"

#include <console.h>

#include <glaze/glaze.hpp>
#include <glaze/json.hpp>

#include <fstream>
#include <alias/filesystem.h>

DEFINE_SINGLETON(SaveManager);

void SaveManager::save(const World& world) {
    auto result = glz::write_json(world);
    if (!result) return print<error, SaveManager>("Could not save world: {}", result.error().custom_error_message);

    if (!filesystem::exists("run")) filesystem::create_directory("run");
    if (auto file = std::ofstream("run/save.json")) file << result.value();
    print<info, SaveManager>("Saved world.");
}

opt<World> SaveManager::load() {
    if (auto file = std::ifstream("run/save.json")) {
        str buf; file >> buf;

        World world;
        auto err = glz::read_json(world, buf);
        if (err) print<error, SaveManager>("Could not load world: {}", err.custom_error_message);
        else { print<info, SaveManager>("Loaded world."); return world; }
    }
    return nullopt;
}