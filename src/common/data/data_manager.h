#pragma once

#include <data/definitions.h>
#include <data/namespaced_id.h>

#include <unordered_map>
#include <filesystem>
#include <util/helper/singleton.h>
#include <util/glaze_prelude.h>

namespace data {
    class Manager { DECL_SINGLETON(Manager);
    public:
        static void reload();

        static const data::Tile* get_tile(const id&);
        static const data::Item* get_item(const id&);

    private:
        static void load_namespace(const std::filesystem::path&);
        static void additive_load_file(const std::string& nmspace, const std::filesystem::path& file);

        struct Registry {
            std::unordered_map<std::string, data::Tile> tiles;
            std::unordered_map<std::string, data::Item> items;
        };
        static_assert(glz::reflectable<Registry>);

        static const Registry* get_namespace(const std::string&);
        std::unordered_map<std::string, Registry> _namespaces;
    };
}