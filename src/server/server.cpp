#include <prelude.h>

#include <data/data_manager.h>
#include <save/save_manager.h>
#include <network/network_manager.h>

#include <terrain/world.h>

#include <game_loop.h>

#include <SFML/Window.hpp>
#include <alias/opt.h>

int main() {
    print<info>("Server init.");

    data::Manager::reload();

    World world = SaveManager::load().or_else([](){ return make_opt<World>(); }).value();

    NetworkManager::connect_listener();

    // Temp window so closing works properly. Should really have a CLI so it can run headless
    sf::Window window = sf::Window(sf::VideoMode(sf::Vector2u(200, 100)), "Server", sf::State::Windowed);

    GameLoop game_loop;
    while (window.isOpen()) {
        window.handleEvents([&window](sf::Event::Closed event){ window.close(); });

        game_loop.tick();
    }
    
    SaveManager::save(world);
}