#include "player_actor.h"

#include <network/network_manager.h>
#include <system/system_manager.h>
#include <actor/actor_manager.h>

PlayerActor::PlayerActor(const str& ident)
: _authority(false), _player_ident(ident), INetworkedActor(::network_id("player"_id, ident)) {
    update_authority_state();
    set_local_rect(DEFAULT_RECT);
}

PlayerActor::PlayerActor(PlayerActor&& rhs) : _authority(rhs._authority), _player_ident(move(rhs._player_ident)), INetworkedActor(move(rhs)) {
    set_local_rect(DEFAULT_RECT);
}

const str& PlayerActor::ident() const { return _player_ident; }
bool PlayerActor::is_authority() const { return _authority; }
void PlayerActor::update_authority_state() {
    auto user_uid = NetworkManager::user_uid();
    _authority = user_uid && (*user_uid == _player_ident);
    print<debug, PlayerActor>("Player {} acting as {}authority.", _player_ident, _authority ? "" : "non-");
}

#ifdef CLIENT
#include <assets/asset_manager.h>
void PlayerActor::draw(sf::RenderTarget& target, draw_layer layer) {
    IActor::draw(target, layer);
    auto& font = AssetManager::get_font("monogram"_id);

    sf::Text player_name_label(font, _player_ident, 16);
    auto label_size = player_name_label.getGlobalBounds().size;
    player_name_label.setOrigin(sf::fvec2(label_size.x / 2, label_size.y));

    fvec2 label_pos = global_rect().origin +
        fvec2(local_rect().size.x / 2, -local_rect().size.y / 2);
    player_name_label.setPosition(to_sfvec(label_pos));

    target.draw(player_name_label);
}
#endif

dyn_arr<LogicalPacket> PlayerActor::get_outstanding_messages() {
    #ifdef CLIENT
    if (!_authority) return {};
    #endif

    if (SystemManager::get_fixed_tick() % 5 != 0) return {};

    /*auto pos_diff = abs(length2(_prev_sent_position - pos()));
    auto vel_diff = abs(length2(_prev_sent_velocity - velocity()));
    //auto accel_diff = abs(length2(_prev_sent_accel - acceleration()));

    if (pos_diff < VALID_DIFF_EPSILON &&
        vel_diff < VALID_DIFF_EPSILON //&&
        //accel_diff < VALID_DIFF_EPSILON
    ) return {};

    _prev_sent_position = pos();
    _prev_sent_velocity = velocity();
    //_prev_sent_accel = acceleration();*/

    LogicalPacket update(network_id(), "motion"_packid);
    update.contents << position().x << position().y << velocity().x << velocity().y << acceleration().x << acceleration().y;
    
    //print<debug, PlayerActor>("Sent motion at {}", update.time);
    return { move(update) };
}

result<success_t, str> PlayerActor::read_message(LogicalPacket&& packet) {
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

        //print<debug, PlayerActor>("{} recieved motion from {} at {}. Tick diff: {}", network_id(), packet.time, SystemManager::get_fixed_tick(), tick_diff);
        auto time_diff = SystemManager::FIXED_TIMESTEP * tick_diff / 1.0s;

        fvec2 interpolated_position = recieved_position + recieved_velocity * time_diff; 
        fvec2 interpolated_velocity = recieved_velocity + recieved_acceleration * time_diff;

        set_position(interpolated_position);
        set_velocity(interpolated_velocity);
        set_acceleration(recieved_acceleration);
        /*set_position(recieved_position);
        set_velocity(recieved_velocity);
        set_acceleration(recieved_acceleration);*/

        // If moving fast enough, perform collision check to ensure actor does not clip through blocks
        float speed = vmath_hpp::length(interpolated_velocity);
        if (speed > 8) ActorManager::handle_collisions(*this);
        return empty_success;
    }
    return err("");
}