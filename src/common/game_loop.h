#pragma once

#include <prelude.h>
#include <alias/chrono.h>

#include <SFML/System/Clock.hpp>

class GameLoop
{
public:
    GameLoop();

    void tick();
private:
    sf::Clock _clock;
    chrono::microseconds _fixed_tick_remainder;
    chrono::microseconds _network_tick_delta;
};