#pragma once

#include <prelude.h>
#include <prelude/containers.h>
#ifdef CLIENT
#include <render/draw_layers.h>
#include <SFML/Graphics.hpp>
#endif

class IDrawable {
public:
    IDrawable(); IDrawable(const IDrawable&); IDrawable(IDrawable&&);
    virtual ~IDrawable();

    #ifdef CLIENT
    virtual dyn_arr<draw_layer> draw_layers() const;
    virtual void draw(sf::RenderTarget&, draw_layer layer) = 0;
    #endif
};