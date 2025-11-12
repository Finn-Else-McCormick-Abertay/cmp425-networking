#include "render_manager.h"
#include <util/console.h>
#include <util/vec_convert.h>

#include <typeinfo>

using namespace std;

DEFINE_SINGLETON(RenderManager);

void RenderManager::Registry::__register(IDrawable& drawable) {
    inst()._added_drawables.insert(&drawable);
}
void RenderManager::Registry::__unregister(IDrawable& drawable) {
    inst()._added_drawables.erase(&drawable);
    inst()._removed_drawables.insert(&drawable);
}

void RenderManager::Registry::__register(Camera& camera) { inst()._cameras.insert(&camera); inst().on_camera_order_changed(); }
void RenderManager::Registry::__unregister(Camera& camera) { inst()._cameras.erase(&camera); inst().on_camera_order_changed(); }

void RenderManager::render() {
    if (!inst()._target) return;

    if (!inst()._added_drawables.empty()) {
        for (auto drawable : inst()._added_drawables) {
            print<debug, RenderManager>("{} registered on layers {}.",
                console_impl::clean_type_name(typeid(*drawable).name()),
                drawable->draw_layers()
            );
            for (auto layer : drawable->draw_layers()) inst()._layers[layer].insert(drawable);
        }
        inst()._added_drawables.clear();
    }
    if (!inst()._removed_drawables.empty()) {
        for (auto drawable : inst()._removed_drawables) {
            for (auto& [layer, contents] : inst()._layers) contents.erase(drawable);
        }
        inst()._removed_drawables.clear();
    }

    inst()._target->clear(sf::Color::Magenta);
    inst().update_target_view();

    for (auto& [layer, drawables] : inst()._layers) {
        for (auto drawable : drawables) drawable->draw(*inst()._target, layer);
    }
}

void RenderManager::on_camera_order_changed() {
    Camera* active_camera_prev = _active_camera; _active_camera = nullptr;
    for (auto camera : _cameras) if (!_active_camera || camera->priority() > _active_camera->priority()) _active_camera = camera;
    //update_target_view();

    if (_active_camera && _active_camera != active_camera_prev)
        print<debug, RenderManager>("Camera switched to '{}'", _active_camera->identifier());
}

void RenderManager::update_target_view() {
    if (_target) _target->setView(_active_camera ? _active_camera->as_view() : _target->getDefaultView());
}

void RenderManager::set_target(sf::RenderTarget* target) {
    inst()._target = target;
    auto target_size = target->getSize();
    inst()._aspect = (float)target_size.y / target_size.x;
}

opt_ref<Camera> RenderManager::active_camera() {
    if (inst()._active_camera) return ref(*inst()._active_camera);
    return nullopt;
}

fvec2 RenderManager::pixel_to_world(const ivec2& pixel) { return to_vec(inst()._target->mapPixelToCoords(to_sfvec(pixel))); }
ivec2 RenderManager::world_to_pixel(const fvec2& point) { return to_vec(inst()._target->mapCoordsToPixel(to_sfvec(point))); }