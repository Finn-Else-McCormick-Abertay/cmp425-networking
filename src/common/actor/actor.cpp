#include "actor.h"

#include <actor/actor_manager.h>

IActor::IActor(const id& id) : _type_id(id) { ActorManager::Registry::__register(*this); }
IActor::~IActor() { ActorManager::Registry::__unregister(*this); }

const id& IActor::type_id() const { return _type_id; }

const ActorTransform& IActor::transform() const { return _transform; }
void IActor::set_transform(const ActorTransform& trans) { _transform = trans; _global_rect_dirty = true; }

const fvec2& IActor::position() const { return _transform.position; }
void IActor::set_position(const fvec2& pos) { _transform.position = pos; _global_rect_dirty = true; }

const fvec2& IActor::velocity() const { return _transform.velocity; }
void IActor::set_velocity(const fvec2& vel) { _transform.velocity = vel; }

const fvec2& IActor::acceleration() const { return _transform.acceleration; }
void IActor::set_acceleration(const fvec2& accel) { _transform.acceleration = accel; }

const frect2& IActor::local_rect() const { return _local_rect; }
void IActor::set_local_rect(const frect2& rect) { _local_rect = rect; _global_rect_dirty = true; }

const frect2& IActor::global_rect() const {
    if (_global_rect_dirty) _global_rect = _local_rect + _transform.position;
    return _global_rect;
}

bool IActor::grounded() const { return _grounded; }
void IActor::set_grounded(bool grounded) { _grounded = grounded; }

#ifdef CLIENT
dyn_arr<draw_layer> IActor::draw_layers() const { return { layers::actor }; }
void IActor::draw(sf::RenderTarget& target, draw_layer layer) {
    auto rect = sf::RectangleShape(to_sfvec(global_rect().size));
    rect.setPosition(to_sfvec(global_rect().origin));
    rect.setFillColor(sf::Color::Yellow);
    target.draw(rect);
}
#endif