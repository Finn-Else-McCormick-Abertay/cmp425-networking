#include <console.h>
#include <cli.h>

#include <data/data_manager.h>
#include <network/network_manager.h>
#include <world/world_manager.h>
#include <save/save_manager.h>

#include <game_loop.h>

#include <SFML/Window.hpp>

int main(int argc, char** argv) {    
    print<info>("Server init.");

    DataManager::reload();
    
    handle_cli(argc, argv, cli::dirs() | cli::world());
    
    NetworkManager::init();
    WorldManager::init();

    // Temp window so closing works properly. Should really have a CLI so it can run headless
    sf::Window window = sf::Window(sf::VideoMode(sf::Vector2u(200, 100)), "Server", sf::State::Windowed);

    GameLoop game_loop;
    while (window.isOpen()) {
        window.handleEvents([&window](sf::Event::Closed event){ window.close(); });
        game_loop.tick();
    }
    
    WorldManager::try_save();
}