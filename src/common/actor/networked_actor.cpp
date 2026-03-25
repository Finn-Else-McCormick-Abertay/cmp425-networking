#include "networked_actor.h"

#include <console.h>
#include <system/system_manager.h>
#include <actor/actor_manager.h>

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
    if (network_mode() == actor::NetworkMode::LISTENER) return {};

    if (SystemManager::get_fixed_tick() % 5 == 0) {
        auto packet_opt = request("motion"_packid);
        if (packet_opt) return { move(packet_opt.value()) };
    }
    return {};
}

result<success_t, str> INetworkedActor::read_message(LogicalPacket&& packet) {
    if (packet.id.type() == "motion") {
        if (is_authority() && !packet.id.has_flag("force")) return empty_success;

        fvec2 recieved_position, recieved_velocity, recieved_acceleration;
        packet.contents >> recieved_position.x >> recieved_position.y >> recieved_velocity.x >> recieved_velocity.y >> recieved_acceleration.x >> recieved_acceleration.y;

        int64 tick_diff = (int64)SystemManager::get_fixed_tick() - (int64)packet.time;
        auto time_diff = SystemManager::FIXED_TIMESTEP * tick_diff / 1.0s;
        
        // Ignore very out of date packets - the physics has been running locally for the actor, and this will be more accurate than an interpolation from an old packet
        if (abs(tick_diff) > 3) {
            print<warning, PlayerActor>("High tick diff of {} : ignoring packet", tick_diff);
            return empty_success;
        }

        actor::InterpolationMode interpolation_mode = ActorManager::interpolation_mode();
        switch (interpolation_mode) {
            case actor::InterpolationMode::NONE: {
                set_position(recieved_position);
            } break;
            case actor::InterpolationMode::LINEAR: {
                fvec2 interpolated_position = recieved_position + recieved_velocity * time_diff; 
                fvec2 interpolated_velocity = recieved_velocity + recieved_acceleration * time_diff;

                fvec2 position_delta = interpolated_position - position();
                
                set_position(interpolated_position);
                set_velocity(interpolated_velocity);
                set_acceleration(recieved_acceleration);
                
                // If moving far enough to potentially clip through tiles, perform collision check
                float position_delta_magnitude = vmath_hpp::length(position_delta);
                //print<debug, INetworkedActor>("Pos diff {}", position_delta_magnitude);
                if (position_delta_magnitude >= 4) {
                    ActorManager::handle_collisions(*this, false);
                }
                
            } break;
            default: return err(fmt::format("Unhandled interpolation mode '{}'.", interpolation_mode));
        }
        return empty_success;
    }
    return err("");
}