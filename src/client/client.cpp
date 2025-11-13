#include <prelude.h>
#include <prelude/opt.h>

#include <window.h>
#include <SFML/Graphics.hpp>

#define __INPUT_ACTION_SYMBOL_DEFINITIONS__
#include <input/actions.h>

#include <input/input_manager.h>
#include <data/data_manager.h>
#include <assets/asset_manager.h>
#include <save/save_manager.h>

#include <terrain/world.h>
#include <player/interaction_system.h>
#include <render/camera.h>

int main() {
    InputManager::init();
    InputManager::setup_default_binds();

    data::Manager::reload();
    
    World world = SaveManager::load().or_else([](){ return make_opt<World>(); }).value();

    auto player_camera = Camera("player");
    auto interaction_system = player::InteractionSystem(&world);

    Window window;
    window.enter_loop();
    
    SaveManager::save(world);
}