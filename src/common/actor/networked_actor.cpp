#include "networked_actor.h"

#include <console.h>
#include <system/system_manager.h>
#include <actor/actor_manager.h>
#include <network/network_manager.h>

INetworkedActor::INetworkedActor(const network_id& network_id, const frect2& rect, actor::PhysicsMode physics_mode, actor::NetworkMode network_mode)
    : _network_mode(network_mode), IActor(network_id.type(), rect, physics_mode), INetworked(network_id) {
    ActorManager::Registry::__register(*this);
}
INetworkedActor::~INetworkedActor() {
    ActorManager::Registry::__unregister(*this);
}
INetworkedActor::INetworkedActor(INetworkedActor&& rhs) : _network_mode(rhs._network_mode), IActor(rhs), INetworked(move(rhs)) {
    ActorManager::Registry::__register(*this);
}

actor::NetworkMode INetworkedActor::network_mode() const { return _network_mode; }
void INetworkedActor::set_network_mode(actor::NetworkMode new_mode) {
    if (new_mode != _network_mode) print<debug, INetworkedActor>("{} '{}' acting as {}.", netid().type(), netid().inst(), new_mode);
    _network_mode = new_mode;
}

bool INetworkedActor::is_authority() const { return network_mode() == actor::NetworkMode::AUTHORITY; }
bool INetworkedActor::is_relay() const { return network_mode() == actor::NetworkMode::RELAY; }

result<LogicalPacket, str> INetworkedActor::get_requested_message(const packet_id& id) const {
    if (id.type() == "motion") {
        LogicalPacket update(netid(), id);
        update.contents << position().x << position().y << velocity().x << velocity().y << acceleration().x << acceleration().y;
        return update;
    }
    return err(fmt::format("Unrecognised packet id {}", id));
}

dyn_arr<LogicalPacket> INetworkedActor::get_outstanding_messages() {
    if (is_authority() && SystemManager::get_fixed_tick() % 5 == 0) {
        auto packet_opt = request("motion"_packid);
        if (packet_opt) return { move(packet_opt.value()) };
    }
    return {};
}

result<success_t, str> INetworkedActor::read_message(LogicalPacket&& packet) {
    if (packet.id.type() == "motion") {
        if (is_authority() && !packet.id.has_flag("force")) return empty_success;
        
        // Rebroadcast
        if (is_relay()) NetworkManager::broadcast(packet);

        MotionInfo recieved {};
        packet.contents >> recieved.position.x >> recieved.position.y >> recieved.velocity.x >> recieved.velocity.y >> recieved.acceleration.x >> recieved.acceleration.y;

        int64 tick_diff = (int64)SystemManager::get_fixed_tick() - (int64)packet.time;
        auto time_diff = SystemManager::FIXED_TIMESTEP * tick_diff / 1.0s;

        Interpolation interpolation = ActorManager::interpolation();
        switch (interpolation) {
            case Interpolation::NONE: {
                set_position(recieved.position);
                set_velocity(fvec2());
                set_acceleration(fvec2());
            } break;
            case Interpolation::MOTION: {
                set_position(recieved.position);
                set_velocity(recieved.velocity);
                set_acceleration(recieved.acceleration);
            } break;
            case Interpolation::LINEAR_POSITION:
            case Interpolation::LINEAR_MOTION: {
                // Ignore very out of date packets - the physics has been running locally for the actor, and this will likely be more accurate than an interpolation from an old packet
                if (abs(tick_diff) >= 5) {
                    // If we haven't accepted a packet in a while, treat it like MOTION
                    if ((SystemManager::get_fixed_tick() - _last_recieved_packet_time) > 20) {
                        set_position(recieved.position);
                        set_velocity(recieved.velocity);
                        set_acceleration(recieved.acceleration);
                    }
                    return empty_success;
                }
                
                // Move actor to interpolated position, doing so in multiple steps with a collision check after each one if distance is great enough to clip through tiles.
                set_position(recieved.position);
                ActorManager::move_actor_respecting_collision(*this, recieved.velocity * time_diff);
                
                if (interpolation == Interpolation::LINEAR_MOTION) {
                    fvec2 interpolated_velocity = recieved.velocity + recieved.acceleration * time_diff;
                    set_velocity(interpolated_velocity);
                    set_acceleration(recieved.acceleration);
                }
                else {
                    set_velocity(fvec2());
                    set_acceleration(fvec2());
                }
            } break;
            default: return err(fmt::format("Unhandled interpolation mode '{}'.", interpolation));
        }
        _last_recieved_packet_time = SystemManager::get_fixed_tick();
        return empty_success;
    }
    return err("");
}