#include "player_actor.h"

#include <network/network_manager.h>
#include <system/system_manager.h>
#include <actor/actor_manager.h>

PlayerActor::PlayerActor(const str& ident, actor::NetworkMode network_mode) : _player_ident(ident), INetworkedActor(
    network_id("player"_id, ident), { { -5, -20 }, { 10, 20 } }, actor::PhysicsMode::DYNAMIC, network_mode) {}

PlayerActor::PlayerActor(PlayerActor&& rhs) : _player_ident(move(rhs._player_ident)), INetworkedActor(move(rhs)) {}

const str& PlayerActor::ident() const { return _player_ident; }

#ifdef CLIENT
#include <assets/asset_manager.h>
void PlayerActor::draw(sf::RenderTarget& target, draw_layer layer) {
    IActor::draw(target, layer);
    auto& font = AssetManager::get_font("monogram"_id);

    sf::Text player_name_label(font, _player_ident, 16);
    auto label_size = player_name_label.getGlobalBounds().size;
    player_name_label.setOrigin(sf::fvec2(label_size.x / 2, label_size.y));

    fvec2 label_pos = global_rect().origin +
        fvec2(rect().size.x / 2, -rect().size.y / 2);
    player_name_label.setPosition(to_sfvec(label_pos));

    target.draw(player_name_label);
}
#endif