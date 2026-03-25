#include "actor.h"

#include <actor/actor_manager.h>

IActor::IActor(const id& id, const frect2& rect, actor::PhysicsMode mode) : _type_id(id), _physics_mode(mode), _rect(rect) { ActorManager::Registry::__register(*this); }

IActor::IActor(const IActor& rhs) : _type_id(rhs._type_id), _physics_mode(rhs._physics_mode), _rect(rhs._rect),
                                    _grounded(rhs._grounded), _position(rhs._position), _velocity(rhs._velocity), _acceleration(rhs._acceleration) {
    ActorManager::Registry::__register(*this);
}
IActor::IActor(IActor&& rhs) : _type_id(move(rhs._type_id)), _physics_mode(rhs._physics_mode), _rect(move(rhs._rect)),
                                _grounded(rhs._grounded), _position(move(rhs._position)), _velocity(move(rhs._velocity)), _acceleration(move(rhs._acceleration)) {
    ActorManager::Registry::__register(*this);
}

IActor::~IActor() { ActorManager::Registry::__unregister(*this); }

const id& IActor::type_id() const                          { return _type_id; }

actor::PhysicsMode IActor::physics_mode() const            { return _physics_mode; }
void IActor::set_physics_mode(actor::PhysicsMode mode)     { _physics_mode = mode; }

const fvec2&  IActor::position() const                     { return _position; }
fvec2&        IActor::position()                           { return _position; }
void          IActor::set_position(const fvec2& pos)       { _position = pos; }

const fvec2&  IActor::velocity() const                     { return _velocity; }
fvec2&        IActor::velocity()                           { return _velocity; }
void          IActor::set_velocity(const fvec2& vel)       { _velocity = vel; }

const fvec2&  IActor::acceleration() const                 { return _acceleration; }
fvec2&        IActor::acceleration()                       { return _acceleration; }
void          IActor::set_acceleration(const fvec2& accel) { _acceleration = accel; }

const frect2& IActor::rect() const                         { return _rect; }
frect2&       IActor::rect()                               { return _rect; }
void          IActor::set_rect(const frect2& rect)         { _rect = rect; }

frect2        IActor::global_rect() const                  { return _rect + _position; }

bool          IActor::grounded() const                     { return _grounded; }
void          IActor::set_grounded(bool grounded)          { _grounded = grounded; }

#ifdef CLIENT
dyn_arr<draw_layer> IActor::draw_layers() const { return { layers::actor }; }
void IActor::draw(sf::RenderTarget& target, draw_layer layer) {
    auto draw_rect = sf::RectangleShape(to_sfvec(rect().size));
    draw_rect.setPosition(to_sfvec(global_rect().origin));
    draw_rect.setFillColor(sf::Color::Yellow);
    target.draw(draw_rect);
}
#endif