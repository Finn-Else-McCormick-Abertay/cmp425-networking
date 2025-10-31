#include "camera_manager.h"
#include <util/console.h>
#include <util/vec_convert.h>

using namespace std;

SINGLETON_INST_DEF(CameraManager)

void CameraManager::Registry::register_camera(Camera& camera) { inst()._cameras.insert(&camera); inst().on_camera_order_changed(); }
void CameraManager::Registry::unregister_camera(Camera& camera) { inst()._cameras.erase(&camera); inst().on_camera_order_changed(); }

void CameraManager::on_camera_order_changed() {
    auto active = get_active_camera();
    if (!active) console::debug("No cameras exist.");
    else console::debug("Camera switched to {}({})", active->identifier(), active->priority());
}

void CameraManager::update_aspect(const uvec2& size) { inst()._aspect = (float)size.y / size.x; }

Camera* CameraManager::get_active_camera() {
    Camera* active = nullptr;
    for (auto camera : inst()._cameras) { if (!active || camera->priority() > active->priority()) active = camera; }
    return active;
}

fvec2 CameraManager::pixel_to_world(const ivec2& pixel) {
    if (Camera* active = get_active_camera()) inst()._target->setView(active->as_view());
    return to_vec(inst()._target->mapPixelToCoords(to_sfvec(pixel)));
}
ivec2 CameraManager::world_to_pixel(const fvec2& point) {
    if (Camera* active = get_active_camera()) inst()._target->setView(active->as_view());
    return to_vec(inst()._target->mapCoordsToPixel(to_sfvec(point)));
}