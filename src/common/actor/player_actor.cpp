#include "player_actor.h"

PlayerActor::PlayerActor(const str& ident) : _player_ident(ident), INetworkedActor(::network_id("player"_id, ident)) {
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
    player_name_label.setOrigin(player_name_label.getGlobalBounds().size / 2.f + player_name_label.getLocalBounds().position);

    auto offset = fvec2(0, 5);
    player_name_label.setPosition(to_sfvec(pos() + local_rect().origin + fvec2(local_rect().size.x / 2, 0) + offset));

    target.draw(player_name_label);
}
#endif