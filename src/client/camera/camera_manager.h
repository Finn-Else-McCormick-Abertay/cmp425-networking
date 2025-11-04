#pragma once

#include <util/helper/singleton.h>
#include <camera/camera.h>
#include <set>
#include <SFML/Graphics/RenderTarget.hpp>

class CameraManager { DECL_SINGLETON(CameraManager);
public:
    DECL_REGISTRY(Camera);

    static Camera* get_active_camera();
    static void update_aspect(const uvec2& window_size);

    static fvec2 pixel_to_world(const ivec2&);
    static ivec2 world_to_pixel(const fvec2&);

private:
    std::set<Camera*> _cameras;
    void on_camera_order_changed();
    friend class Camera; friend class Window;

    float _aspect = 1.f;
    sf::RenderTarget* _target;
};

