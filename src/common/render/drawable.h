#pragma once

#include <prelude.h>
#include <prelude/containers.h>
#ifdef CLIENT
#include <SFML/Graphics.hpp>
#endif

using draw_layer = int16;

namespace layers {
    inline constexpr draw_layer unknown = 0;

    namespace tile {
        inline constexpr draw_layer foreground = 10;
        inline constexpr draw_layer background = 5;
    }

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

class IDrawable {
public:
    IDrawable();
    IDrawable(const IDrawable&); IDrawable(IDrawable&&);
    virtual ~IDrawable();

    virtual dyn_arr<draw_layer> draw_layers() const;

    #ifdef CLIENT
    virtual void draw(sf::RenderTarget&, draw_layer layer) = 0;
    #endif
};