#include <util/console.h>
#include <util/prelude.h>

#include <window.h>
#include <input/input_manager.h>

#include <SFML/Graphics.hpp>
#include <terrain/world.h>
#include <player/interaction_system.h>
#include <render/camera.h>

#define __INPUT_ACTION_SYMBOL_DEFINITIONS__
#include <input/actions.h>

#include <glaze/json.hpp>

#include <util/helper/enum_serialization.h>

#include <data/data_manager.h>
#include <data/namespaced_id.h>

#include <assets/asset_manager.h>
#include <type_traits>

#include <save/save_manager.h>

#include <util/std_aliases.h>
#include <ranges>

using namespace std;

int main() {
    InputManager::init();
    InputManager::setup_default_binds();

    data::Manager::reload();
    
    World world = SaveManager::load().or_else([](){ return std::make_optional<World>(); }).value();

    auto player_camera = Camera("player");
    auto interaction_system = player::InteractionSystem(&world);

    //for (auto& id : data::Manager::tile_ids()) print<info>("{} -> {}", id, data::Manager::id_mapping(id));
    //for (auto& id : data::Manager::item_ids()) print<info>("{} -> {}", id, data::Manager::id_mapping(id));

    Window window;
    window.enter_loop();
    
    SaveManager::save(world);
}