#include "save_manager.h"

#include <console.h>

#include <glaze/glaze.hpp>
#include <glaze/json.hpp>

#include <alias/filesystem.h>

DEFINE_SINGLETON(SaveManager);

const fs::path& SaveManager::user_folder() { return inst()._user_folder; }
void SaveManager::set_user_folder(const fs::path& path) {
    auto normalised_path = path.lexically_normal();
    if (normalised_path != inst()._user_folder) print<info, SaveManager>("User path set to '{}'.", normalised_path);
    inst()._user_folder = path;
}

result<success_t, str> SaveManager::save_world(const World& world) {
    if (!fs::exists(user_folder())) return err(fmt::format("User directory '{}' invalid.", user_folder()));
    if (world.name().empty()) return err(fmt::format("World name was empty."));

    auto world_root = user_folder() / "saves" / world.name();
    fs::create_directories(world_root); // Create any intermediate directories

    auto path = world_root / "world.json";

    auto result = glz::write_json(world);
    if (!result) return err(fmt::format("Serialisation failed: {}", result.error().custom_error_message));
    
    if (auto file = ofstream(path)) file << result.value();
    else return err(fmt::format("Failed to write to save file '{}'.", path));

    return empty_success;
}

result<World, str> SaveManager::load_world(const str& world_name) {
    if (!fs::exists(user_folder())) return err(fmt::format("User directory '{}' invalid.", user_folder()));
    if (world_name.empty()) return err(fmt::format("World name was empty."));

    auto world_root = user_folder() / "saves" / world_name;
    auto path = world_root / "world.json";

    if (!fs::exists(path)) return err(fmt::format("Save file '{}' invalid.", path));

    str buffer;
    if (auto file = ifstream(path)) file >> buffer;
    else return err(fmt::format("Failed to read from save file '{}'.", path));
    
    print<debug, World>(buffer);

    auto result = glz::read_json<World>(buffer);
    if (!result) return err(fmt::format("Deserialisation failed: {}", result.error().custom_error_message));
    return move(*result);
}