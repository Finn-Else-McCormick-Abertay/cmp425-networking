#include "save_manager.h"

#include <util/console.h>

#include <glaze/glaze.hpp>
#include <glaze/json.hpp>

#include <fstream>
#include <filesystem>

using namespace std;

DEFINE_SINGLETON(SaveManager);

void SaveManager::save(const World& world) {
    auto result = glz::write_json(world);
    if (!result) return print<error, SaveManager>("Could not save world: {}", result.error().custom_error_message);

    if (!filesystem::exists("run")) filesystem::create_directory("run");
    if (auto file = ofstream("run/save.json")) file << result.value();
    print<info, SaveManager>("Saved world.");
}

opt<World> SaveManager::load() {
    if (auto file = ifstream("run/save.json")) {
        str buf; file >> buf;
        //print<debug, SaveManager>("Save file contents: {}", buf);

        //Chunk chunk;
        //auto err = glz::read_json(chunk, R"({"chunk_coords":[0,0], "tiles":[default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air,default::air]})");

        World world;
        auto err = glz::read_json(world, buf);
        if (err) print<error, SaveManager>("Could not load world: {}", err.custom_error_message);
        else {
            print<info, SaveManager>("Loaded world.");
            return world;
        }
    }
    return nullopt;
}