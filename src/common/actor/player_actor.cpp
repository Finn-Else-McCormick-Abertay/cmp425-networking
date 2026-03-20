#include "player_actor.h"

#include <network/network_manager.h>

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
    //return {};
    #ifdef CLIENT
    if (!_authority) return {};
    #endif

    auto pos_diff = abs(length2(_prev_sent_position - pos()));
    auto vel_diff = abs(length2(_prev_sent_velocity - velocity()));
    auto accel_diff = abs(length2(_prev_sent_accel - acceleration()));

    if (pos_diff < VALID_DIFF_EPSILON && vel_diff < VALID_DIFF_EPSILON && accel_diff < VALID_DIFF_EPSILON) return {};

    _prev_sent_position = pos();
    _prev_sent_velocity = velocity();
    _prev_sent_accel = acceleration();

    //print<debug, PlayerActor>("Diff {} {} {}", pos_diff, vel_diff, accel_diff);

    auto& pos_send = pos(); auto& vel_send = velocity(); auto& accel_send = acceleration();

    //print<debug, PlayerActor>("Sent motion data {} {} {}", pos_send, vel_send, accel_send);
    LogicalPacket update(packet_id("motion"));
    update.packet << pos_send.x << pos_send.y << vel_send.x << vel_send.y << accel_send.x << accel_send.y;
    return { move(update) };
}

result<success_t, str> PlayerActor::read_message(LogicalPacket&& packet) {
    if (packet.id.type() == "motion") {
        #ifdef CLIENT
        if (_authority) return empty_success;
        #endif

        float pos_x, pos_y, vel_x, vel_y, acc_x, acc_y;

        packet.packet >> pos_x >> pos_y >> vel_x >> vel_y >> acc_x >> acc_y;
        
        //print<debug>("Recieved motion data {} {}", pos_x, pos_y);

        set_pos(fvec2(pos_x, pos_y));
        set_velocity(fvec2(vel_x, vel_y));
        set_acceleration(fvec2(acc_x, acc_y));
        return empty_success;
    }
    return err("");
}