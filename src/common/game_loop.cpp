#include "game_loop.h"

#include <SFML/System/Time.hpp>

#include <system/system_manager.h>
#include <network/network_manager.h>
#include <actor/actor_manager.h>

GameLoop::GameLoop() {
    _fixed_tick_remainder = chrono::microseconds::zero();
    _clock.stop();
}

void GameLoop::tick() {
    // Tick systems
    sf::Time delta_time = _clock.restart();
    SystemManager::tick(delta_time.asSeconds());

    ActorManager::tick(delta_time.asSeconds());

    // Tick physics
    auto fixed_rate_delta_time = delta_time.toDuration() + _fixed_tick_remainder;
    uint fixed_rate_steps = fixed_rate_delta_time / FIXED_TIMESTEP;
    _fixed_tick_remainder = fixed_rate_delta_time % FIXED_TIMESTEP;

    for (uint i = 0; i < fixed_rate_steps; ++i) {
        // (The physics doesn't actually run in the fixed tick cause I ran into problems with it too late to fix them)
        SystemManager::fixed_tick(_elapsed_ticks);
        _elapsed_ticks++;
    }

    // Tick network
    NetworkManager::network_tick(_elapsed_ticks);
}