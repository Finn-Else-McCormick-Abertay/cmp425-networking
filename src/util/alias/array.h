#pragma once

#include <array>
#include <vector>

template<typename T, size_t Size> using arr = std::array<T, Size>;

template<typename T> using dyn_arr = std::vector<T>;