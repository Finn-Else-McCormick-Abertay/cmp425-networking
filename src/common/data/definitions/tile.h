#pragma once

#include <prelude/data_definition.h>

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