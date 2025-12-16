#pragma once

#include <expected>

struct none_t { constexpr explicit none_t(int) {} };
struct success_t { constexpr explicit success_t(int) {} };

inline constexpr none_t none {0};
inline constexpr success_t empty_success {0};

template<typename T, typename E> using result = std::expected<T, E>;
template<typename E> using err = std::unexpected<E>;