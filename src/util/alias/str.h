#pragma once

#include <string>
#include <string_view>

using str = std::string;
using str_view = std::string_view;

#include <alias/concepts.h>

#include <charconv>

template<typename T> requires integral<T> || floating_point<T>
constexpr T str_to(const str& s) {
    T result; std::from_chars(s.data(), s.data() + s.size(), result);
    return result;
}