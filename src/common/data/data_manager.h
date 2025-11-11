#pragma once

#include <data/namespaced_id.h>
#include <data/definitions/tile.h>
#include <data/definitions/item.h>
#include <data/handle/tile.h>
#include <data/handle/item.h>

#include <map>
#include <filesystem>
#include <util/helper/singleton.h>
#include <util/glaze_prelude.h>
#include <util/primitive_aliases.h>
#include <util/std_aliases.h>
#include <ranges>

namespace data {
    class Manager { DECL_SINGLETON(Manager);
    public:
        static void reload();

        static opt_cref<TileHandle> get_tile(const id&);
        static opt_cref<ItemHandle> get_item(const id&);

        static inline auto tile_ids() { return std::views::keys(inst()._tile_handles); }
        static inline auto item_ids() { return std::views::keys(inst()._item_handles); }

        static uint32 id_mapping(const data::id&);
        static const data::id& mapped_id(uint32);

    private:
        std::map<data::id, TileHandle> _tile_handles;
        std::map<data::id, ItemHandle> _item_handles;

        void map_id(const data::id&, uint32);

        std::map<uint32, data::id> _mapped_to_ids;
        std::map<data::id, uint32> _ids_to_mapped;
    };
}