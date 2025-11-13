#pragma once

#include <optional>

template<typename T> using opt = std::optional<T>;
using std::nullopt;

// Forward 'make_opt' to 'std::make_optional'

template<class T, std::enable_if_t<std::is_constructible_v<std::decay_t<T>, T>, int> = 0>
constexpr opt<std::decay_t<T>> make_opt(T&& value) noexcept {
    return std::make_optional<T>(std::forward<T>(value));
}

template <class T, class... Args,
    std::enable_if_t<std::is_constructible_v<T, Args...>, int> = 0>
constexpr opt<T> make_opt(Args&&... args) noexcept {
    return std::make_optional<T>(std::forward<Args>(args)...);
}

template <class T, class Elem, class... Args,
    std::enable_if_t<std::is_constructible_v<T, std::initializer_list<Elem>&, Args...>, int> = 0>
constexpr opt<T> make_opt(std::initializer_list<Elem> ilist, Args&&... args) noexcept {
    return std::make_optional<T>(ilist, std::forward<Args>(args)...);
}