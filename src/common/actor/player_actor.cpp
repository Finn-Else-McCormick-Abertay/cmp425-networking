#include "player_actor.h"

#include <network/network_manager.h>
#include <system/system_manager.h>
#include <actor/actor_manager.h>
#include <random>

PlayerActor::PlayerActor(const str& ident, const str& display_name, actor::NetworkMode network_mode) : _player_ident(ident), _player_display_name(display_name), INetworkedActor(
    network_id("player"_id, ident), { { -5, -20 }, { 10, 20 } }, actor::PhysicsMode::DYNAMIC, network_mode
) {
    // Generate debug color, seeded by name so it's consistent
    auto name_random = std::mt19937(std::hash<str>()(display_name));

    static constexpr int16 color_range[3] = { 80, -200, -150 };
    uint8 generated_color[3] = {};
    for (uint i = 0; i < 3; ++i) {
        int16 range = color_range[i];

        uint generated = name_random() % abs(color_range[i]);
        if (range < 0) generated += (255 + range);

        generated_color[i] = generated;
    }
    
    set_debug_color(fmt::rgb(generated_color[0], generated_color[1], generated_color[2]));
}

PlayerActor::PlayerActor(PlayerActor&& rhs) : _player_ident(move(rhs._player_ident)), _player_display_name(move(rhs._player_display_name)), INetworkedActor(move(rhs)) {}

const str& PlayerActor::ident() const { return _player_ident; }
const str& PlayerActor::display_name() const { return _player_display_name; }

#ifdef CLIENT
#include <assets/asset_manager.h>
void PlayerActor::draw(sf::RenderTarget& target, draw_layer layer) {
    IActor::draw(target, layer);
    auto& font = AssetManager::get_font("monogram"_id);

    sf::Text player_name_label(font, display_name(), 16);

    auto label_size = player_name_label.getGlobalBounds().size;
    player_name_label.setOrigin(sf::fvec2(label_size.x / 2, label_size.y));

    fvec2 label_pos = global_rect().origin + fvec2(rect().size.x / 2, -rect().size.y / 2 - 1.f);

    sf::Color main_color = sf::Color::White;
    
    auto rgb = fmt::rgb(debug_color());
    sf::Color shadow_color = sf::Color(rgb.r, rgb.g, rgb.b) * sf::Color(130,130,130);
    
    player_name_label.setPosition(to_sfvec(label_pos + fvec2(1, 1)));
    player_name_label.setFillColor(shadow_color);
    target.draw(player_name_label);

    player_name_label.setPosition(to_sfvec(label_pos));
    player_name_label.setFillColor(main_color);
    target.draw(player_name_label);
}
#endif