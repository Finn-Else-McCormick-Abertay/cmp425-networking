#pragma once

#include <prelude.h>

using draw_layer = int16;

namespace layers {
    inline constexpr draw_layer unknown = 0;

    namespace tile {
        inline constexpr draw_layer background = 5;
        inline constexpr draw_layer foreground = 10;
    }
    
    inline constexpr draw_layer actor = 11;
    inline constexpr draw_layer player = 12;

    namespace ui {
        inline constexpr draw_layer start = 35; // Layers from here and up are drawn in ui space
        inline constexpr draw_layer hud = 40;
        inline constexpr draw_layer menu = 50;

    }
    
    namespace debug {
        inline constexpr draw_layer world_overlay = 20;
        inline constexpr draw_layer ui_overlay = 60;
    }
}