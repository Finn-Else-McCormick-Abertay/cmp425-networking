#pragma once

#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Joystick.hpp>

namespace Mouse {
    using sf::Mouse::Button;
    using sf::Mouse::Wheel;
    enum Axis { Motion };
}

using Key = sf::Keyboard::Key;
using ScanCode = sf::Keyboard::Scan;

namespace Controller {
    using sf::Joystick::Axis;

    enum class Button {
        FACE_BOTTOM = 0, // XBOX A
        FACE_RIGHT = 1,  // XBOX B
        FACE_LEFT = 2,   // XBOX X
        FACE_TOP = 3,    // XBOX Y

        XBOX_A = FACE_BOTTOM,
        XBOX_B = FACE_RIGHT,
        XBOX_X = FACE_LEFT,
        XBOX_Y = FACE_TOP,

        LB = 4,
        RB = 5,
        MENU = 6,
        START = 7,
        LEFT_STICK = 8,
        RIGHT_STICK = 9,
    };
}