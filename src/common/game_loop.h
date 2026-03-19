#pragma once

#include <prelude.h>
#include <alias/chrono.h>

#include <SFML/System/Clock.hpp>

class GameLoop
{
public:
    GameLoop();

    void tick();

    // 60 fixed-rate steps per second
    static constexpr chrono::microseconds FIXED_TIMESTEP = 16ms;
    
    // Set a minimum time between network ticks to avoid the sockets getting overwhelmed
    static constexpr chrono::microseconds NETWORK_MIN_TIMESTEP = 5ms;
private:
    sf::Clock _clock;
    chrono::microseconds _fixed_tick_remainder;
    chrono::microseconds _network_tick_delta;

    uint64 _elapsed_fixed_ticks;
    uint64 _elapsed_network_ticks;
};