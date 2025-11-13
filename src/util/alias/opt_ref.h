#pragma once

#include <alias/opt.h>
#include <functional>

template<typename T> using opt_ref = std::optional<std::reference_wrapper<T>>;
template<typename T> using opt_cref = std::optional<std::reference_wrapper<const T>>;

using std::ref;
using std::cref;
