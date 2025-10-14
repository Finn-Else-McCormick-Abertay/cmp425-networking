#pragma once
#include <util/vec.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

template<typename T> sf::Vector2<T> to_sf(const vmath_hpp::vec<T, 2>& vec) { return { vec.at(0), vec.at(1) }; }
template<typename T> sf::Vector3<T> to_sf(const vmath_hpp::vec<T, 3>& vec) { return { vec.at(0), vec.at(1), vec.at(2) }; }


template<typename T> vmath_hpp::vec<T, 2> to_vec(const sf::Vector2<T>& vec) { return { vec.x, vec.y }; }
template<typename T> vmath_hpp::vec<T, 3> to_vec(const sf::Vector3<T>& vec) { return { vec.x, vec.y, vec.z }; }