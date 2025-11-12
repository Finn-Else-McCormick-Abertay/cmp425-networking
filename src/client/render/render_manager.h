#pragma once

#include <util/helper/singleton.h>
#include <render/camera.h>
#include <render/drawable.h>
#include <set>
#include <map>
#include <SFML/Graphics/RenderTarget.hpp>
#include <util/std_aliases.h>

class RenderManager { DECL_SINGLETON(RenderManager);
public:
    DECL_MULTI_REGISTRY(IDrawable, Camera);

    static void render();

    static opt_ref<Camera> active_camera();

    static fvec2 pixel_to_world(const ivec2&);
    static ivec2 world_to_pixel(const fvec2&);

private:
    friend class Camera; friend class Window;

    std::set<IDrawable*> _added_drawables, _removed_drawables;
    std::map<uint, std::set<IDrawable*>> _layers;

    std::set<Camera*> _cameras; Camera* _active_camera;
    void on_camera_order_changed();
    void update_target_view();

    sf::RenderTarget* _target; float _aspect = 1.f;
    static void set_target(sf::RenderTarget*);
};

