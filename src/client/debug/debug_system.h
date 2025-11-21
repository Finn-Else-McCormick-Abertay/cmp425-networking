#pragma once

#include <terrain/world.h>
#include <system/system.h>
#include <render/drawable.h>

class DebugSystem : ITickingSystem, IDrawable {
public:
    DebugSystem(World* world);
    virtual void tick(float dt) override;
    
    virtual dyn_arr<draw_layer> draw_layers() const override;
    virtual void draw(sf::RenderTarget&, draw_layer layer) override;

private:
    World* _world;

    bool _show_debug = false;
};