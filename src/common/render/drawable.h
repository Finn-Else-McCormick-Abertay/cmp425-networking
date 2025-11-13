#pragma once

#include <prelude.h>
#include <prelude/containers.h>
#ifdef CLIENT
#include <SFML/Graphics.hpp>
#endif

namespace layers {
    static constexpr uint DEFAULT = 0;
    static constexpr uint TILE_FOREGROUND = 10;
    static constexpr uint TILE_BACKGROUND = 5;
}

class IDrawable {
public:
    IDrawable();
    IDrawable(const IDrawable&); IDrawable(IDrawable&&);
    virtual ~IDrawable();

    virtual dyn_arr<uint> draw_layers() const;

    #ifdef CLIENT
    virtual void draw(sf::RenderTarget&, uint layer) = 0;
    #endif
};