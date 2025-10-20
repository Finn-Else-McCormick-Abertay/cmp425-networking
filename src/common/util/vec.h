#pragma once

#include <vmath.hpp/vmath_vec.hpp>
#include <vmath.hpp/vmath_vec_fun.hpp>
#include <util/stream/vec.h>

template<typename T, int Size> using vec = vmath_hpp::vec<T, Size>;
template<typename T> using vec2 = vec<T, 2>;
template<typename T> using vec3 = vec<T, 3>;
template<typename T> using vec4 = vec<T, 4>; 
using vmath_hpp::fvec2;
using vmath_hpp::fvec3;
using vmath_hpp::fvec4;

using vmath_hpp::ivec2;
using vmath_hpp::ivec3;
using vmath_hpp::ivec4;

using vmath_hpp::uvec2;
using vmath_hpp::uvec3;
using vmath_hpp::uvec4;

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
// Alias for the SFML vec types
using sf_fvec2 = sf::Vector2f;
using sf_fvec3 = sf::Vector3f;

using sf_ivec2 = sf::Vector2i;
using sf_ivec3 = sf::Vector3i;

using sf_uvec2 = sf::Vector2u;