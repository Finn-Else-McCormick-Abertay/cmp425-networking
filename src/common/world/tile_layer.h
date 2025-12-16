#pragma once

#include <prelude.h>
#include <util/helper/glaze_enum_helper.h>

enum class tile_layer : uint8 {
    Unknown,
    Foreground,
    Background,
};

ENUM_GLAZE_AND_FORMATTER(tile_layer, (Background, Foreground));