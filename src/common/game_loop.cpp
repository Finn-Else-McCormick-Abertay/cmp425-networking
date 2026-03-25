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
    SystemManager::perform_tick(delta_time.asSeconds());

    // Calculate number of fixed ticks to perform this frame
    auto fixed_rate_delta_time = delta_time.toDuration() + _fixed_tick_remainder;
    uint fixed_rate_steps = fixed_rate_delta_time / SystemManager::FIXED_TIMESTEP;
    _fixed_tick_remainder = fixed_rate_delta_time % SystemManager::FIXED_TIMESTEP;

    // Tick physics
    for (uint i = 0; i < fixed_rate_steps; ++i) SystemManager::perform_fixed_tick();
    // Tick network
    if (fixed_rate_steps > 0) NetworkManager::perform_network_tick();
}