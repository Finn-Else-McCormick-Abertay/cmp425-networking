#pragma once

#include <util/std_aliases.h>
#include <util/glaze_prelude.h>
#include <util/helper/format.h>
#include <fmt/std.h>
#include <unordered_map>

namespace data { namespace definition {
    enum class ModelType{ none, block };
    enum class CollisionType { none, block };

    struct Tile {
        opt<str> texture;
        variant<ModelType, str> model = ModelType::block;
        CollisionType collision = CollisionType::block;
    };
    static_assert(glz::reflectable<Tile>);
}}
ENUM_GLAZE_AND_FORMATTER(data::definition::ModelType, (none, block), use_type_name = false);
ENUM_GLAZE_AND_FORMATTER(data::definition::CollisionType, (none, block), use_type_name = false);