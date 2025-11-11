#pragma once

#include <string>
#include <optional>
#include <functional>
#include <variant>

using str = std::string;

template<typename T> using opt = std::optional<T>;
using std::nullopt;

template<typename T> using opt_ref = std::optional<std::reference_wrapper<T>>;
template<typename T> using opt_cref = std::optional<std::reference_wrapper<const T>>;

using std::variant;
