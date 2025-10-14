#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <iostream>

template<typename T> std::ostream& operator<<(std::ostream& os, const sf::Vector2<T>& vec) { return os << '(' << vec.x << ", " << vec.y << ')'; }
template<typename T> std::ostream& operator<<(std::ostream& os, const sf::Vector3<T>& vec) { return os << '(' << vec.x << ", " << vec.y << ", " << vec.z << ')'; }