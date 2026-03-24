#include "networked_actor.h"

#include <console.h>
#include <system/system_manager.h>
#include <actor/actor_manager.h>

INetworkedActor::INetworkedActor(const network_id& network_id) : IActor(network_id.type()), INetworked(network_id) {}
INetworkedActor::INetworkedActor(INetworkedActor&& rhs) : _authority(rhs._authority), IActor(rhs.netid().type()), INetworked(move(rhs)) {}

bool INetworkedActor::is_authority() const { return _authority; }
void INetworkedActor::set_authority(bool val) {
    _authority = val;
    print<debug, INetworkedActor>("{} acting as {}authority.", netid(), _authority ? "" : "non-");
}

dyn_arr<LogicalPacket> INetworkedActor::get_outstanding_messages() {
    #ifdef CLIENT
    if (!_authority) return {};
    #endif

    if (SystemManager::get_fixed_tick() % 5 != 0) return {};

    LogicalPacket update(netid(), "motion"_packid);
    update.contents << position().x << position().y << velocity().x << velocity().y << acceleration().x << acceleration().y;
    
    //print<debug, PlayerActor>("Sent motion at {}", update.time);
    return { move(update) };
}

result<success_t, str> INetworkedActor::read_message(LogicalPacket&& packet) {
    if (packet.id.type() == "motion") {
        #ifdef CLIENT
        if (_authority) return empty_success;
        #endif

        float pos_x, pos_y, vel_x, vel_y, accel_x, accel_y;
        packet.contents >> pos_x >> pos_y >> vel_x >> vel_y >> accel_x >> accel_y;

        auto recieved_position = fvec2(pos_x, pos_y);
        auto recieved_velocity = fvec2(vel_x, vel_y);
        auto recieved_acceleration = fvec2(accel_x, accel_y);

        int64 tick_diff = (int64)SystemManager::get_fixed_tick() - (int64)packet.time;
        // Ignore very out of date packets - the physics has been running locally for the actor, and this will be more accurate than an interpolation from an old packet
        if (abs(tick_diff) > 3) {
            print<warning, PlayerActor>("High tick diff of {} : ignoring packet", tick_diff);
            return empty_success;
        }

        //print<debug, PlayerActor>("{} recieved motion from {} at {}. Tick diff: {}", netid(), packet.time, SystemManager::get_fixed_tick(), tick_diff);
        auto time_diff = SystemManager::FIXED_TIMESTEP * tick_diff / 1.0s;

        fvec2 interpolated_position = recieved_position + recieved_velocity * time_diff; 
        fvec2 interpolated_velocity = recieved_velocity + recieved_acceleration * time_diff;

        fvec2 position_change = interpolated_position - position();

        set_position(interpolated_position);
        set_velocity(interpolated_velocity);
        set_acceleration(recieved_acceleration);

        // If moving far enough to potentially clip through tiles, perform collision check
        if (vmath_hpp::length(position_change) > 8) ActorManager::handle_collisions(*this);
        return empty_success;
    }
    return err("");
}