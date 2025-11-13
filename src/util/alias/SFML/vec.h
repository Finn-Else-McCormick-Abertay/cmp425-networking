#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
// Alias for the SFML vec types

namespace sf {
    template<typename T> using vec2 = sf::Vector2<T>;
    template<typename T> using vec3 = sf::Vector3<T>;

    using fvec2 = sf::Vector2f;
    using fvec3 = sf::Vector3f;

    using ivec2 = sf::Vector2i;
    using ivec3 = sf::Vector3i;

    using uvec2 = sf::Vector2u;
}