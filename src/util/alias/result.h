#pragma once

#include <expected>

struct none_t { constexpr explicit none_t(int) {} };
inline constexpr none_t none {0};

template<typename T, typename E> using result = std::expected<T, E>;
template<typename E> using err = std::unexpected<E>;

//template<typename E> using err_result = std::expected<none_t, E>;