#include "actor.h"

#include <actor/actor_manager.h>

IActor::IActor(const id& id) : _type_id(id) { ActorManager::Registry::__register(*this); }
IActor::~IActor() { ActorManager::Registry::__unregister(*this); }

const id& IActor::type_id() const { return _type_id; }

const fvec2& IActor::pos() const { return _pos; }
void IActor::set_pos(const fvec2& pos) { _pos = pos; _global_rect_dirty = true; }

const fvec2& IActor::velocity() const { return _velocity; }
void IActor::set_velocity(const fvec2& vel) { _velocity = vel; }

const fvec2& IActor::acceleration() const { return _accel; }
void IActor::set_acceleration(const fvec2& accel) { _accel = accel; }

const frect2& IActor::local_rect() const { return _local_rect; }
void IActor::set_local_rect(const frect2& rect) { _local_rect = rect; _global_rect_dirty = true; }

const frect2& IActor::global_rect() const {
    if (_global_rect_dirty) _global_rect = _local_rect + _pos;
    return _global_rect;
}

#ifdef CLIENT
dyn_arr<draw_layer> IActor::draw_layers() const { return { layers::actor }; }
void IActor::draw(sf::RenderTarget& target, draw_layer layer) {
    auto rect = sf::RectangleShape(to_sfvec(local_rect().size));
    rect.setPosition(to_sfvec(pos()));
    rect.setFillColor(sf::Color::Yellow);
    target.draw(rect);
}
#endif

INetworkedActor::INetworkedActor(const ::network_id& network_id) : IActor(network_id.type()), INetworked(network_id) {}
INetworkedActor::INetworkedActor(INetworkedActor&& rhs) : IActor(rhs.network_id().type()), INetworked(move(rhs)) {}

/*
dyn_arr<LogicalPacket> INetworkedActor::get_outstanding_messages() {
    LogicalPacket update(packet_id("motion"));
    update.packet << pos().x << pos().y << velocity().x << velocity().y << acceleration().x << acceleration().y;
    return { move(update) };
}

result<success_t, str> INetworkedActor::read_message(LogicalPacket&& packet) {
    if (packet.id.type() == "motion") {
        fvec2 pos, velocity, acceleration;

        packet.packet >> pos.x >> pos.y >> velocity.x >> velocity.y >> acceleration.x >> acceleration.y;

        set_pos(pos);
        set_velocity(velocity);
        set_acceleration(acceleration);
        return empty_success;
    }
    return err("");
}
    */