#pragma once

#include <vmath.hpp/vmath_vec.hpp>
#include <iostream>
#include <sstream>
#include <format>

template<typename T, int Size>
std::ostream& operator<<(std::ostream& os, const vmath_hpp::vec<T, Size>& vec) {
    os << '(';
    for (auto it = vec.cbegin(); it != vec.cend();) { os << *it; if (++it != vec.cend()) os << ", "; }
    os << ')';
    return os;
}

template<typename T, int Size>
struct std::formatter<vmath_hpp::vec<T, Size>> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const vmath_hpp::vec<T, Size>& vec, std::format_context& ctx) const {
        std::stringstream ss;
        for (auto it = vec.cbegin(); it != vec.cend();) { ss << *it; if (++it != vec.cend()) ss << ", "; }
        return std::format_to(ctx.out(), "({})", ss.str());
    }
};