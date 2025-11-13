#pragma once

#include <SFML/Graphics/RenderTarget.hpp>

#include <render/camera.h>
#include <render/drawable.h>

#include <prelude.h>
#include <prelude/opt.h>
#include <prelude/containers.h>
#include <util/helper/singleton.h>

class RenderManager { DECL_SINGLETON(RenderManager);
public:
    DECL_MULTI_REGISTRY(IDrawable, Camera);

    static void render();

    static opt_ref<Camera> active_camera();

    static fvec2 pixel_to_world(const ivec2&);
    static ivec2 world_to_pixel(const fvec2&);

private:
    friend class Camera; friend class Window;

    set<IDrawable*> _added_drawables, _removed_drawables;
    ordered_map<uint, set<IDrawable*>> _layers;

    set<Camera*> _cameras; Camera* _active_camera;
    void on_camera_order_changed();
    void update_target_view();

    sf::RenderTarget* _target; float _aspect = 1.f;
    static void set_target(sf::RenderTarget*);
};

