#pragma once
#include <util/vec.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

// Convert between SFML vec and vmath vec

template<typename T> sf::Vector2<T> to_sfvec(const vec<T, 2>& vec) { return { vec.at(0), vec.at(1) }; }
template<typename T> sf::Vector3<T> to_sfvec(const vec<T, 3>& vec) { return { vec.at(0), vec.at(1), vec.at(2) }; }


template<typename T> vec<T, 2> to_vec(const sf::Vector2<T>& vec) { return { vec.x, vec.y }; }
template<typename T> vec<T, 3> to_vec(const sf::Vector3<T>& vec) { return { vec.x, vec.y, vec.z }; }

// Convert between vecs of different value types

template<typename T_Output, typename T_Input> sf::Vector2<T_Output> to_sfvec_of(const vec<T_Input, 2>& vec)
    { return { static_cast<T_Output>(vec.at(0)), static_cast<T_Output>(vec.at(1)) }; }
template<typename T_Output, typename T_Input> sf::Vector3<T_Output> to_sfvec_of(const vec<T_Input, 3>& vec)
    { return { static_cast<T_Output>(vec.at(0)), static_cast<T_Output>(vec.at(1)), static_cast<T_Output>(vec.at(2)) }; }


template<typename T_Output, typename T_Input> sf::Vector2<T_Output> to_sfvec_of(const sf::Vector2<T_Input>& vec)
    { return { static_cast<T_Output>(vec.x), static_cast<T_Output>(vec.y) }; }
template<typename T_Output, typename T_Input> sf::Vector3<T_Output> to_sfvec_of(const sf::Vector3<T_Input>& vec)
    { return { static_cast<T_Output>(vec.x), static_cast<T_Output>(vec.y), static_cast<T_Output>(vec.z) }; }


template<typename T_Output, typename T_Input> vec<T_Output, 2> to_vec_of(const sf::Vector2<T_Input>& vec)
    { return { static_cast<T_Output>(vec.x), static_cast<T_Output>(vec.y) }; }
template<typename T_Output, typename T_Input> vec<T_Output, 3> to_vec_of(const sf::Vector3<T_Input>& vec)
    { return { static_cast<T_Output>(vec.x), static_cast<T_Output>(vec.y), static_cast<T_Output>(vec.z) }; }


template<typename T_Output, typename T_Input, size_t Size> vec<T_Output, Size> to_vec_of(const vec<T_Input, Size>& input_vec) {
    vec<T_Output, Size> output_vec = {};
    for (size_t i = 0; i < Size; ++i) output_vec[i] = static_cast<T_Output>(input_vec[i]);
    return output_vec;
}

#define DEF_TO_VEC_ALIAS(prefix, type)\
    template<typename T_Input> vec<type, 2> to_##prefix##vec(const sf::Vector2<T_Input>& vec) { return to_vec_of<type>(vec); }\
    template<typename T_Input> vec<type, 3> to_##prefix##vec(const sf::Vector3<T_Input>& vec) { return to_vec_of<type>(vec); }\
    template<typename T_Input, size_t Size> vec<type, Size> to_##prefix##vec(const vec<T_Input, Size>& vec) { return to_vec_of<type>(vec); }

DEF_TO_VEC_ALIAS(f, float)
DEF_TO_VEC_ALIAS(i, int)
DEF_TO_VEC_ALIAS(u, unsigned)