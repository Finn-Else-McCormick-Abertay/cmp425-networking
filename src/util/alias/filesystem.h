#pragma once

#include <filesystem>

using filepath = std::filesystem::path;

using dir = std::filesystem::directory_iterator;
using dir_and_below = std::filesystem::recursive_directory_iterator;

namespace filesystem {
    using namespace std::filesystem;
}