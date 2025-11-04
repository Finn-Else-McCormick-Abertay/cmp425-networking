#pragma once

#include <util/glaze_prelude.h>
#include <util/helper/format.h>
#include <fmt/std.h>
#include <unordered_map>

namespace data {
    enum class DefaultModel{ none, block };
    using Model = std::variant<DefaultModel, std::string>;
}
ENUM_GLAZE_AND_FORMATTER(data::DefaultModel, (none, block), use_type_name = false);

namespace data {
    struct Tile {
        std::string display_name;
        bool has_item = true;

        std::optional<std::string> texture;
        Model model = DefaultModel::block;

        enum class Collision { none, block };
        Collision collision = Collision::block;
    };
    static_assert(glz::reflectable<Tile>);
}
ENUM_GLAZE_AND_FORMATTER(data::Tile::Collision, (none, block), use_type_name = false);

namespace data {
    struct Item {
        std::string display_name;

        std::optional<std::string> texture;
    };
    static_assert(glz::reflectable<Item>);
}