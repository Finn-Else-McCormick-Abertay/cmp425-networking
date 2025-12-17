#include "player_actor.h"

#include <network/network_manager.h>

PlayerActor::PlayerActor(const str& ident, bool listen_only)
: _listen_only(listen_only), _player_ident(ident), INetworkedActor(::network_id("player"_id, ident)) {
    set_local_rect(DEFAULT_RECT);
}

PlayerActor::PlayerActor(PlayerActor&& rhs) : _player_ident(move(rhs._player_ident)), INetworkedActor(move(rhs)) {
    set_local_rect(DEFAULT_RECT);
}

#ifdef CLIENT
#include <assets/asset_manager.h>
void PlayerActor::draw(sf::RenderTarget& target, draw_layer layer) {
    IActor::draw(target, layer);
    auto& font = AssetManager::get_font("monogram"_id);

    sf::Text player_name_label(font, _player_ident, 16);
    auto size = player_name_label.getGlobalBounds().size;
    player_name_label.setOrigin(sf::fvec2(size.x / 2, 0));

    auto offset = fvec2(local_rect().size.x, 0);
    player_name_label.setPosition(to_sfvec(pos() + local_rect().origin + offset));

    target.draw(player_name_label);
}
#endif

dyn_arr<LogicalPacket> PlayerActor::get_outstanding_messages() {
    #ifdef CLIENT
    if (_listen_only) return {};
    #endif
    //print<debug>("Sent motion data {}", pos());
    LogicalPacket update(packet_id("motion"));
    update.packet << pos().x << pos().y << velocity().x << velocity().y << acceleration().x << acceleration().y;
    return { move(update) };
}

result<success_t, str> PlayerActor::read_message(LogicalPacket&& packet) {
    if (packet.id.type() == "motion") {
        float pos_x, pos_y, vel_x, vel_y, acc_x, acc_y;

        packet.packet >> pos_x >> pos_y >> vel_x >> vel_y >> acc_x >> acc_y;
        
        //print<debug>("Recieved motion data {} {}", pos_x, pos_y);

        set_pos(fvec2(pos_x, pos_y));
        set_velocity(fvec2(vel_x, vel_y));
        //set_acceleration(fvec2(acc_x, acc_y));
        return empty_success;
    }
    return err("");
}