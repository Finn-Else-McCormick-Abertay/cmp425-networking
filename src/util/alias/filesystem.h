#pragma once

#include <filesystem>
#include <fstream>

namespace fs {
    using namespace std::filesystem;

    using dir = std::filesystem::directory_iterator;
    using dir_recursive = std::filesystem::recursive_directory_iterator;
}

using std::fstream;
using std::ifstream;
using std::ofstream;