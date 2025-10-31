#include <util/console.h>
#include <util/prelude.h>

#include <window.h>
#include <input/input_manager.h>

#include <SFML/Graphics.hpp>
#include <terrain/world.h>
#include <player/interaction_system.h>
#include <camera/camera.h>

#include <map>
#include <iostream>

#define __INPUT_ACTION_SYMBOL_DEFINITIONS__
#include <input/actions.h>

using namespace std;

int main() {
    console::info("Client init");
    InputManager::init();
    InputManager::setup_default_binds();
    
    auto world = World();

    auto player_camera = Camera("player");
    auto interaction_system = player::InteractionSystem(&world);

    Window window;

    sf::Texture placeholder_texture = sf::Texture("resources/assets/textures/placeholder.png");
    placeholder_texture.setRepeated(true);
    
    sf::Texture tileset = sf::Texture("resources/assets/textures/tileset/template.png");
    tileset.setSmooth(false);

    window.set_close_request_callback([](Window& window) -> bool {
        console::debug("Window: Close Requested");
        return true;
    });
    window.set_draw_callback([&](sf::RenderTarget& target) {
        auto chunk_true_size = Chunk::SIZE_TILES * Tile::SIZE;

        auto chunk_debug_rect = sf::RectangleShape(sf_fvec2(chunk_true_size - 2, chunk_true_size - 2));
        chunk_debug_rect.setOutlineColor(sf::Color(255, 255, 255, 100));
        chunk_debug_rect.setOutlineThickness(1);
        chunk_debug_rect.setFillColor(sf::Color::Transparent);

        auto make_tile_rect = [](sf::Color fill, sf::Color outline = sf::Color::Transparent) -> sf::RectangleShape {
            sf::RectangleShape rect(sf_fvec2(Tile::SIZE, Tile::SIZE));
            rect.setFillColor(fill); rect.setOutlineColor(outline); rect.setOutlineThickness(1);
            return rect;
        };
        map<Tile::Type, sf::RectangleShape> tile_rects = {
            {Tile::Air, make_tile_rect(sf::Color::Transparent)},
            {Tile::Stone, make_tile_rect(sf::Color(255, 255, 255))},
        };
        tile_rects.at(Tile::Stone).setTexture(&tileset);

        for (auto& chunk : world) {
            ivec2 chunk_true_coords(chunk.get_coords() * chunk_true_size);
            
            // Draw debug rect
            chunk_debug_rect.setPosition(to_sfvec_of<float>(chunk_true_coords + ivec2(1, 1)));
            target.draw(chunk_debug_rect);

            // Draw tiles
            for (auto [local_pos, tile] : chunk) {
                if (tile->type() == Tile::Air) continue;
                auto& rect = tile_rects.at(tile->type());
                rect.setPosition(to_sfvec_of<float>(chunk_true_coords + to_fvec(local_pos) * Tile::SIZE));

                uint8 shape = (uint8)tile->shape();
                uvec2 texture_tile_index = uvec2(shape % 6, shape / 6);
                rect.setTextureRect(sf::IntRect(to_sfvec_of<int>(texture_tile_index * Tile::SIZE), to_sfvec(ivec2(Tile::SIZE, Tile::SIZE))));
                target.draw(rect);
            }
        }
    });

    window.enter_main_loop();
}