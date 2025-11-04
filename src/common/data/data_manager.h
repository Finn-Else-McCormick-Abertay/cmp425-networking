#pragma once

#include <data/definitions.h>

#include <unordered_map>
#include <util/helper/singleton.h>

namespace data {
    class Manager { DECL_SINGLETON(Manager);
    public:
        //DECL_REGISTRY(int, friend);

    private:
        //std::unordered_map<std::string, std::unordered_map<std::string, TileType>> _tile_definitions;
    };
}