#include <console.h>
#include <window.h>
#include <cli.h>

#include <data/data_manager.h>
#include <network/network_manager.h>
#include <world/world_manager.h>
#include <save/save_manager.h>

#define __INPUT_ACTION_SYMBOL_DEFINITIONS__
#include <input/actions.h>
#include <input/input_manager.h>
#include <assets/asset_manager.h>

#include <player/interaction_system.h>
#include <debug/debug_system.h>
#include <render/camera.h>

#include <util/format/SFML/network.h>

int main(int argc, char** argv) {
    print<info>("Client init.");

    DataManager::reload();

    handle_cli(argc, argv, cli::dirs() | cli::client());

    if (!NetworkManager::server_address()) NetworkManager::connect_to_server(sf::IpAddress::LocalHost, 2s);
    
    InputManager::init();
    InputManager::setup_default_binds();

    WorldManager::init();

    // - TK: move to player actor
    auto player_camera = Camera("player");

    // Systems
    auto interaction_system = player::InteractionSystem();
    auto debug_system = DebugSystem();

    Window window;
    window.enter_loop();

    NetworkManager::disconnect_all();
}