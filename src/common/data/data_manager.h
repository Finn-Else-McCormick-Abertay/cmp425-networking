#pragma once

#include <data/namespaced_id.h>
#include <data/definitions/tile.h>
#include <data/definitions/item.h>
#include <data/handle/tile.h>
#include <data/handle/item.h>

#include <prelude.h>
#include <prelude/filesystem.h>
#include <prelude/containers.h>
#include <ranges>

#include <util/helper/singleton.h>

namespace data {
    class Manager { DECL_SINGLETON(Manager);
    public:
        static void reload();

        static opt_cref<TileHandle> get_tile(const id&);
        static opt_cref<ItemHandle> get_item(const id&);

        static inline auto tile_ids() { return std::views::keys(inst()._tile_handles); }
        static inline auto item_ids() { return std::views::keys(inst()._item_handles); }

        static uint32 id_mapping(const id&);
        static const id& mapped_id(uint32);

    private:
        hashmap<id, TileHandle> _tile_handles;
        hashmap<id, ItemHandle> _item_handles;

        void map_id(const id&, uint32);
        hashmap<uint32, id> _mapped_to_ids; hashmap<id, uint32> _ids_to_mapped;
    };
}