#pragma once

#include <world/level.h>
#include <system/system.h>
#include <render/drawable.h>

class DebugSystem : ITickingSystem, IDrawable {
public:
    virtual void tick(float dt) override;
    
    virtual dyn_arr<draw_layer> draw_layers() const override;
    virtual void draw(sf::RenderTarget&, draw_layer layer) override;

private:
    bool _show_tile_debug = false;
    bool _show_network_debug = true;
};